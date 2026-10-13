"""
Name:        structure.py
Description: turns a function's blocks and jumps into if / else, loops, switch, break and continue
Version:     0.1.0
Created:     11-10-2026
Modified:    12-10-2026
Author:      Sebastian Toyos

The control flow graph is read with dominators. A block a back edge jumps to heads a loop: its body is
every block that gets back to it without passing through it. A loop is left by break, to the block
the loop goes on at (the exit most of its exits share), or by return. A two-way branch is an if whose
two sides meet again at the block that follows it on every path (its immediate post-dominator). A small
block that only ends the function (a few statements and a return) is copied into every place that
jumps to it, the way C reads: "if (x) { ...; return; }". So is a small piece of code a jump leads into
when everything from there on is small and never comes back to it (TAIL_COPY): it is written again
where the jump is. What still does not fit (a jump into the middle of something already written, an
irreducible loop) stays a goto to a label; a goto that ends a branch, to the label the code reaches
anyway when the branch ends, goes.

The result is a tree of the node classes below; emit.py prints it.
"""
import copy
from dataclasses import dataclass, field

from v60 import ir

SMALL_EXIT = 4    # statements of an ending block that is copied rather than jumped to
TAIL_COPY = 20    # statements (and block ends) of all the code a jump leads to, at most, to write it again


# ---- the tree ----

@dataclass
class Code:
    label: object
    statements: list


@dataclass
class If:
    condition: object
    then: list
    otherwise: list = field(default_factory=list)


@dataclass
class Loop:
    body: list
    kind: str = "forever"         # "forever", "while" (condition before the body) or "do" (after it)
    condition: object = None


@dataclass
class Switch:
    value: object
    cases: list                   # [(case values, body)]


@dataclass
class End:
    """The end of the function: a Return, a TailCall or an InterruptReturn."""
    end: object


@dataclass
class Goto:
    label: object


@dataclass
class Label:
    label: object


class Break:
    pass


class Continue:
    pass


# ---- graph facts ----

EXIT = "exit"


def successors(block):
    return block.successors()


def reverse_postorder(blocks, entry):
    order = []
    seen = set()
    stack = [(entry, iter(successors(blocks[entry])))]
    seen.add(entry)
    while stack:
        label, children = stack[-1]
        advanced = False
        for child in children:
            if child not in seen:
                seen.add(child)
                stack.append((child, iter(successors(blocks[child]))))
                advanced = True
                break
        if not advanced:
            order.append(label)
            stack.pop()
    order.reverse()
    return order


def dominators(order, predecessors, entry):
    """Immediate dominators (Cooper, Harvey, Kennedy)."""
    index = {label: position for position, label in enumerate(order)}
    idom = {entry: entry}

    def intersect(a, b):
        while a != b:
            while index[a] > index[b]:
                a = idom[a]
            while index[b] > index[a]:
                b = idom[b]
        return a

    changed = True
    while changed:
        changed = False
        for label in order[1:]:
            done = [p for p in predecessors[label] if p in idom and p in index]
            if not done:
                continue
            new = done[0]
            for other in done[1:]:
                new = intersect(other, new)
            if idom.get(label) != new:
                idom[label] = new
                changed = True
    return idom


def dominates(idom, a, b):
    """True if a dominates b."""
    while True:
        if a == b:
            return True
        parent = idom.get(b)
        if parent is None or parent == b:
            return False
        b = parent


class Graph:
    def __init__(self, function):
        self.blocks = function.blocks
        self.entry = function.entry
        self.order = reverse_postorder(self.blocks, self.entry)
        self.position = {label: index for index, label in enumerate(self.order)}
        self.predecessors = {label: [] for label in self.order}
        for label in self.order:
            for successor in successors(self.blocks[label]):
                self.predecessors[successor].append(label)
        self.idom = dominators(self.order, self.predecessors, self.entry)
        self.loops = self.find_loops()
        self.ipdom = self.post_dominators()

    def find_loops(self):
        """{header: (body, latches)} for the natural loops; a back edge to a block that does not dominate
        its source (an irreducible loop) heads none."""
        latches = {}
        for label in self.order:
            for successor in successors(self.blocks[label]):
                if self.position[successor] <= self.position[label] and dominates(self.idom, successor, label):
                    latches.setdefault(successor, []).append(label)
        loops = {}
        for header, sources in latches.items():
            body = {header}
            stack = [source for source in sources if source != header]
            body |= set(stack)
            while stack:
                label = stack.pop()
                for predecessor in self.predecessors[label]:
                    if predecessor not in body:
                        body.add(predecessor)
                        stack.append(predecessor)
            loops[header] = (body, sources)
        return loops

    def post_dominators(self):
        """Immediate post-dominators, with EXIT after every block that ends the function. A loop nothing
        leaves is taken to leave from its header, so every block reaches EXIT. A small block that ends the
        function is written where it is jumped to ("if (x) { ...; return; }"), so a way into one does not
        count: what the other ways meet at is where the code goes on."""
        def written_in_place(label):
            block = self.blocks[label]
            return isinstance(block.end, (ir.Return, ir.TailCall, ir.InterruptReturn)) \
                and len(block.statements) <= SMALL_EXIT
        def kept(label):
            targets = list(successors(self.blocks[label]))
            # an ending one of the other ways also runs into is where they meet ("if (x) { ... }" and then it)
            joined = {after for target in targets for after in successors(self.blocks[target])}
            onward = [target for target in targets if not written_in_place(target) or target in joined]
            # a loop's way out stays: leaving the loop is where its body goes on
            if any(self.position[target] > self.position[label] for target in onward):
                return onward
            return targets
        succ = {label: kept(label) for label in self.order}
        for label in self.order:
            if not succ[label]:
                succ[label].append(EXIT)
        reaches = self.reaching_exit(succ)
        for header in sorted(self.loops, key=lambda label: self.position[label]):
            if header not in reaches:
                succ[header].append(EXIT)
                reaches = self.reaching_exit(succ)
        for label in self.order:
            if label not in reaches:
                succ[label].append(EXIT)
        reverse_preds = {label: [] for label in self.order}
        reverse_preds[EXIT] = []
        for label, targets in succ.items():
            for target in targets:
                reverse_preds.setdefault(target, [])
        # the reverse graph: EXIT first, edges backwards
        rsucc = {label: [] for label in list(self.order) + [EXIT]}
        rpred = {label: [] for label in list(self.order) + [EXIT]}
        for label, targets in succ.items():
            for target in targets:
                rsucc[target].append(label)
                rpred[label].append(target)
        order = []
        seen = {EXIT}
        stack = [(EXIT, iter(rsucc[EXIT]))]
        while stack:
            label, children = stack[-1]
            advanced = False
            for child in children:
                if child not in seen:
                    seen.add(child)
                    stack.append((child, iter(rsucc[child])))
                    advanced = True
                    break
            if not advanced:
                order.append(label)
                stack.pop()
        order.reverse()
        return dominators(order, rpred, EXIT)

    def reaching_exit(self, succ):
        reverse = {}
        for label, targets in succ.items():
            for target in targets:
                reverse.setdefault(target, []).append(label)
        found = set()
        stack = [EXIT]
        while stack:
            label = stack.pop()
            for source in reverse.get(label, []):
                if source not in found:
                    found.add(source)
                    stack.append(source)
        return found


def split_loop_entries(function, notes):
    """A loop the code enters at two places (an unrolled loop whose odd count starts at its second half)
    has no header C can write. When one of the ways in is a small block whose only way on is the other
    one, the code that comes from outside gets a copy of that block: the loop then starts at the other
    block, and the copy runs before it. 'notes' as for split_small_exits."""
    blocks = function.blocks
    changed = True
    rounds = 0
    while changed and rounds < 8:
        changed = False
        rounds += 1
        graph = Graph(function)
        for component in strongly_connected(blocks, graph.order):
            if len(component) < 2:
                continue
            entries = {label for label in component
                       if label == function.entry or any(source not in component
                                                         for source in graph.predecessors.get(label, ()))}
            if len(entries) != 2 or function.entry in entries:
                continue
            for small in sorted(entries, key=str):
                other = (entries - {small}).pop()
                block = blocks[small]
                if len(block.statements) > SMALL_EXIT or not isinstance(block.end, ir.Goto) \
                        or block.end.target != other:
                    continue
                outside = [source for source in graph.predecessors[small] if source not in component]
                made = {}
                twin = copy.copy(block)
                twin.label = ("entry", small)
                twin.statements = [copy.deepcopy(statement, made) for statement in block.statements]
                twin.end = copy.deepcopy(block.end, made)
                for note in notes:
                    for key in [key for key in note if isinstance(key, int) and key in made]:
                        note[id(made[key])] = copied_note(note[key], made)
                    if ("end", id(block)) in note:
                        note[("end", id(twin))] = note[("end", id(block))]
                blocks[twin.label] = twin
                for source in outside:
                    retarget(blocks[source].end, small, twin.label)
                changed = True
                break
            if changed:
                break


def retarget(end, old, new):
    """Makes a block end that jumps to 'old' jump to 'new' instead."""
    if isinstance(end, ir.Goto) and end.target == old:
        end.target = new
    elif isinstance(end, ir.Branch):
        if end.target == old:
            end.target = new
        if end.otherwise == old:
            end.otherwise = new
    elif isinstance(end, ir.Switch):
        end.cases = {case: new if target == old else target for case, target in end.cases.items()}


def strongly_connected(blocks, order):
    """The strongly connected components of the blocks reachable from the entry (Tarjan)."""
    index, low, on_stack, stack, found = {}, {}, set(), [], []
    counter = [0]

    def visit(start):
        work = [(start, iter(successors(blocks[start])))]
        index[start] = low[start] = counter[0]
        counter[0] += 1
        stack.append(start)
        on_stack.add(start)
        while work:
            label, children = work[-1]
            advanced = False
            for child in children:
                if child not in index:
                    index[child] = low[child] = counter[0]
                    counter[0] += 1
                    stack.append(child)
                    on_stack.add(child)
                    work.append((child, iter(successors(blocks[child]))))
                    advanced = True
                    break
                if child in on_stack:
                    low[label] = min(low[label], index[child])
            if advanced:
                continue
            work.pop()
            if work:
                low[work[-1][0]] = min(low[work[-1][0]], low[label])
            if low[label] == index[label]:
                component = set()
                while True:
                    member = stack.pop()
                    on_stack.discard(member)
                    component.add(member)
                    if member == label:
                        break
                found.append(component)

    for label in order:
        if label not in index:
            visit(label)
    return found


def copied_note(value, made):
    """A note about a node, for its copy: the nodes it names (a field's base register) are the copy's."""
    if isinstance(value, tuple):
        return tuple(made.get(id(part), part) if not isinstance(part, (int, str, bool, type(None))) else part
                     for part in value)
    return value


def split_small_exits(function, notes):
    """A small block that ends the function and is jumped to from several places gets a copy of its
    own for each (they are written where they are jumped to anyway): each copy then knows which values
    reach it, so a result handed back as it came in is not copied back. 'notes' are the dicts keyed by
    the id of a statement or ("end", id of a block) (cleanup.py's liveness, pointers.py's fields),
    filled in for the copies."""
    thread_jumps(function)
    join_conditions(function)    # "if (a || b) return": one way in after all
    predecessors = {}
    for label, block in function.blocks.items():
        for successor in successors(block):
            predecessors.setdefault(successor, []).append(label)
    for label in list(function.blocks):
        block = function.blocks[label]
        sources = predecessors.get(label, [])
        if not isinstance(block.end, (ir.Return, ir.TailCall)) or len(block.statements) > SMALL_EXIT \
                or len(sources) < 2 or label == function.entry:
            continue
        for number, source in enumerate(dict.fromkeys(sources[1:]), 2):
            # a deep copy: the twin's expressions are its own nodes, so what is noted about a node (the
            # variable a register read finds, the field a load reads) can differ between the copies
            made = {}
            twin = copy.copy(block)
            twin.label = ("exit", label, number)
            twin.statements = [copy.deepcopy(statement, made) for statement in block.statements]
            twin.end = copy.deepcopy(block.end, made)
            for note in notes:
                for key in [key for key in note if isinstance(key, int) and key in made]:
                    note[id(made[key])] = copied_note(note[key], made)
                if ("end", id(block)) in note:
                    note[("end", id(twin))] = note[("end", id(block))]
            function.blocks[twin.label] = twin
            end = function.blocks[source].end
            if isinstance(end, ir.Goto) and end.target == label:
                end.target = twin.label
            elif isinstance(end, ir.Branch):
                if end.target == label:
                    end.target = twin.label
                if end.otherwise == label:
                    end.otherwise = twin.label
            elif isinstance(end, ir.Switch):
                end.cases = {case: twin.label if target == label else target for case, target in end.cases.items()}


# ---- building the tree ----

def thread_jumps(function):
    """A jump to a block that does nothing but jump on goes straight to where that one leads (the V60
    tables of "br" instructions are made of such blocks)."""
    blocks = function.blocks

    def final(label):
        seen = set()
        while label != function.entry and label not in seen:
            block = blocks[label]
            if block.statements or not isinstance(block.end, ir.Goto):
                break
            seen.add(label)
            label = block.end.target
        return label

    for block in blocks.values():
        end = block.end
        if isinstance(end, ir.Goto):
            end.target = final(end.target)
        elif isinstance(end, ir.Branch):
            end.target = final(end.target)
            end.otherwise = final(end.otherwise)
        elif isinstance(end, ir.Switch):
            end.cases = {case: final(target) for case, target in end.cases.items()}


def join_conditions(function):
    """"if (a) goto X; if (b) goto X; ..." is one test: if (a || b). A block that only branches, that only
    one block jumps to, and that shares a target with that block, goes into its condition (|| or &&, which
    evaluate in the same order and skip what the jump skipped)."""
    blocks = function.blocks
    changed = True
    while changed:
        changed = False
        predecessors = {}
        for label, block in blocks.items():
            for successor in block.successors():
                predecessors.setdefault(successor, []).append(label)
        for label, block in blocks.items():
            end = block.end
            if not isinstance(end, ir.Branch):
                continue
            for side in ("otherwise", "target"):
                inner_label = getattr(end, side)
                inner = blocks.get(inner_label)
                if inner is None or inner_label == label or inner.statements or not isinstance(inner.end, ir.Branch) \
                        or predecessors.get(inner_label) != [label] or inner_label == function.entry:
                    continue
                shared = end.target if side == "otherwise" else end.otherwise
                c1, c2 = end.condition, inner.end.condition
                if side == "otherwise":
                    # if (c1) X; else if (c2 ...) -> one of inner's targets is X
                    if inner.end.target == shared:
                        new = Branch_parts(logical("lor", c1, c2), shared, inner.end.otherwise)
                    elif inner.end.otherwise == shared:
                        new = Branch_parts(logical("lor", c1, negate(c2)), shared, inner.end.target)
                    else:
                        continue
                else:
                    # if (c1) { if (c2 ...) } else Y -> one of inner's targets is Y
                    if inner.end.otherwise == shared:
                        new = Branch_parts(logical("land", c1, c2), inner.end.target, shared)
                    elif inner.end.target == shared:
                        new = Branch_parts(logical("land", c1, negate(c2)), inner.end.otherwise, shared)
                    else:
                        continue
                end.condition, end.target, end.otherwise = new
                del blocks[inner_label]
                changed = True
                break
            if changed:
                break


def Branch_parts(condition, target, otherwise):
    return condition, target, otherwise


def logical(name, a, b):
    return ir.op(name, (a, b), 1)


@dataclass
class LoopContext:
    header: object
    body: set            # the natural loop
    region: set          # the body, and code only the loop leads to that never comes back: written inside
    follow: object       # where the loop goes on
    in_switch: bool = False

    def inside_switch(self):
        return LoopContext(self.header, self.body, self.region, self.follow, True)


class Structurer:
    def __init__(self, function):
        thread_jumps(function)
        join_conditions(function)
        self.function = function
        self.blocks = function.blocks
        self.graph = Graph(function)
        self.emitted = set()
        self.labels = set()        # blocks something jumps to with goto
        self.pending = []          # of those, the ones not written yet

    def small_exit(self, label):
        block = self.blocks[label]
        return (isinstance(block.end, (ir.Return, ir.TailCall, ir.InterruptReturn))
                and len(block.statements) <= SMALL_EXIT)

    def loop_follow(self, header):
        body, _ = self.graph.loops[header]
        exits = {}
        for label in body:
            for successor in successors(self.blocks[label]):
                if successor not in body:
                    exits[successor] = exits.get(successor, 0) + 1
        candidates = [label for label in exits if not self.small_exit(label)] or list(exits)
        if not candidates:
            return None
        # the exit that the most code after the loop hangs from: it reaches the most blocks
        return max(candidates, key=lambda label: (len(self.reachable(label)), exits[label], -self.graph.position[label]))

    def reachable(self, start):
        found = {start}
        stack = [start]
        while stack:
            label = stack.pop()
            for successor in successors(self.blocks[label]):
                if successor not in found:
                    found.add(successor)
                    stack.append(successor)
        return found

    def build(self):
        tree = self.region(self.graph.entry, None, None)
        # code only gotos reach goes after the rest, each piece ending the function or jumping back
        while self.pending:
            label = self.pending.pop(0)
            if label in self.emitted:
                continue
            tree += self.region(label, None, None)
        return tree

    def copy_tail(self, target):
        """The code from 'target' on, written again: when all of it is small, never comes back to
        'target' and needs no goto of its own. None otherwise."""
        reach = self.reachable(target)
        if any(target in successors(self.blocks[label]) for label in reach):
            return None
        if sum(len(self.blocks[label].statements) + 1 for label in reach) > TAIL_COPY:
            return None
        copy = Structurer.__new__(Structurer)
        copy.function, copy.blocks, copy.graph = self.function, self.blocks, self.graph
        copy.emitted, copy.labels, copy.pending = set(), set(), []
        nodes = copy.region(target, None, None)
        if copy.labels:
            return None
        return without_labels(nodes)

    def goto(self, target):
        copied = self.copy_tail(target)
        if copied is not None:
            return copied
        self.labels.add(target)
        if target not in self.emitted:
            self.pending.append(target)
        return [Goto(target)]

    def jump(self, target, stop, loop):
        """What going to 'target' is from inside 'loop' with the region ending at 'stop': a list of nodes,
        and the block the walk goes on at (None when the jump ends the walk)."""
        if loop is not None:
            if target == loop.header:
                return [Continue()], None
            if target not in loop.region:
                if target == loop.follow:
                    # inside a switch, break would only leave the switch
                    return (self.goto(target) if loop.in_switch else [Break()]), None
                if self.small_exit(target):
                    return self.copy_exit(target), None
                block = self.blocks[target]
                if not loop.in_switch and isinstance(block.end, ir.Goto) and block.end.target == loop.follow \
                        and len(block.statements) <= SMALL_EXIT and target != self.function.entry:
                    # a few statements on the way out of the loop: written where the jump is, then break
                    return [Code(target, list(block.statements)), Break()], None
                return self.goto(target), None
        if target == stop:
            return [], None
        if target in self.emitted:
            if self.small_exit(target):
                return self.copy_exit(target), None
            return self.goto(target), None
        return [], target

    def copy_exit(self, label):
        block = self.blocks[label]
        return [Code(label, list(block.statements)), End(block.end)]

    def region(self, start, stop, loop):
        """Nodes for the code from 'start' up to 'stop' (not included) inside 'loop' (a LoopContext or
        None)."""
        out = []
        current = start
        while current is not None and current != stop:
            if (loop is not None and current not in loop.region) or current in self.emitted:
                nodes, _ = self.jump(current, stop, loop)
                out += nodes
                break
            if current in self.graph.loops and (loop is None or loop.header != current):
                current = self.loop(current, out, stop, loop)
                continue
            current = self.block(current, out, stop, loop)
        return out

    def loop(self, header, out, stop, outer):
        body, _ = self.graph.loops[header]
        follow = self.loop_follow(header)
        after = self.reachable(follow) if follow is not None else set()
        region = set(body)
        for label in self.graph.order:
            if label in body or label in after or (outer is not None and label in outer.body):
                continue
            if dominates(self.graph.idom, header, label):
                region.add(label)
        inner = LoopContext(header, body, region, follow)
        nodes = []
        current = self.block(header, nodes, None, inner)
        if current is not None:
            nodes += self.region(current, None, inner)
        out.append(Loop(nodes))
        if follow is None:
            return None
        nodes, next_label = self.jump(follow, stop, outer)
        out += nodes
        return next_label

    def block(self, label, out, stop, loop):
        """Writes one block and its end; returns where the walk goes on."""
        self.emitted.add(label)
        block = self.blocks[label]
        out.append(Label(label))
        if block.statements:
            out.append(Code(label, list(block.statements)))
        end = block.end
        if isinstance(end, (ir.Return, ir.TailCall, ir.InterruptReturn)):
            out.append(End(end))
            return None
        if isinstance(end, ir.Goto):
            nodes, next_label = self.jump(end.target, stop, loop)
            out += nodes
            return next_label
        if isinstance(end, ir.Branch):
            return self.branch(label, end, out, stop, loop)
        if isinstance(end, ir.Switch):
            return self.switch(label, end, out, stop, loop)
        raise ValueError(f"block end {end!r}")

    def follow_of(self, label, stop, loop):
        """Where the two or more ways out of 'label' meet again inside the current region, or None."""
        follow = self.graph.ipdom.get(label)
        if follow == EXIT or follow is None or follow == label:
            return None
        if loop is not None and (follow not in loop.region or follow == loop.header):
            return None
        if follow != stop and follow in self.emitted:
            return None
        return follow

    def branch(self, label, end, out, stop, loop):
        then_label, else_label = end.target, end.otherwise
        # an early way out: "if (x) { ...; return; }" and the walk goes on with the other side
        for exit_label, other, condition in ((then_label, else_label, end.condition),
                                            (else_label, then_label, negate(end.condition))):
            if loop is not None and exit_label == loop.follow:
                continue    # leaving the loop: a break, and the way out is written once after it
            if self.small_exit(exit_label) and not self.small_exit(other) and exit_label != stop \
                    and exit_label not in successors(self.blocks[other]):
                out.append(If(condition, self.copy_exit(exit_label)))
                nodes, next_label = self.jump(other, stop, loop)
                out += nodes
                return next_label
        follow = self.follow_of(label, stop, loop)
        region_stop = follow if follow is not None else stop
        then_nodes = self.side(then_label, region_stop, loop)
        else_nodes = self.side(else_label, region_stop, loop)
        if not then_nodes and else_nodes:
            out.append(If(negate(end.condition), else_nodes))
        else:
            out.append(If(end.condition, then_nodes, else_nodes))
        if follow is None:
            return None
        nodes, next_label = self.jump(follow, stop, loop)
        out += nodes
        return next_label

    def side(self, label, stop, loop):
        """One side of an if: the walk from 'label' until 'stop'."""
        if label == stop:
            return []
        nodes, next_label = self.jump(label, stop, loop)
        if next_label is None:
            return nodes
        return nodes + self.region(next_label, stop, loop)

    def switch(self, label, end, out, stop, loop):
        follow = self.follow_of(label, stop, loop)
        region_stop = follow if follow is not None else stop
        by_target = {}
        for case, target in end.cases.items():
            by_target.setdefault(target, []).append(case)
        cases = []
        inner = loop.inside_switch() if loop is not None else None
        for target, values in sorted(by_target.items(), key=lambda item: min(item[1])):
            body = self.side(target, region_stop, inner)
            if not ends_flow(body):
                body.append(Break())
            cases.append((sorted(values), body))
        out.append(Switch(end.address, cases))
        if follow is None:
            return None
        nodes, next_label = self.jump(follow, stop, loop)
        out += nodes
        return next_label


def without_labels(nodes):
    """'nodes' with their labels taken out (a copy must not repeat them)."""
    found = []
    for node in nodes:
        if isinstance(node, Label):
            continue
        if isinstance(node, If):
            node = If(node.condition, without_labels(node.then), without_labels(node.otherwise))
        elif isinstance(node, Loop):
            node = Loop(without_labels(node.body), node.kind, node.condition)
        elif isinstance(node, Switch):
            node = Switch(node.value, [(values, without_labels(body)) for values, body in node.cases])
        found.append(node)
    return found


def ends_flow(nodes):
    """True if control never runs off the end of 'nodes'."""
    if not nodes:
        return False
    last = nodes[-1]
    if isinstance(last, (End, Goto, Break, Continue)):
        return True
    if isinstance(last, If):
        return bool(last.otherwise) and ends_flow(last.then) and ends_flow(last.otherwise)
    if isinstance(last, Loop):
        return last.kind == "forever" and not contains_break(last.body)
    return False


def contains_break(nodes):
    for node in nodes:
        if isinstance(node, Break):
            return True
        if isinstance(node, If) and (contains_break(node.then) or contains_break(node.otherwise)):
            return True
        if isinstance(node, Switch):
            continue    # a break there leaves the switch
    return False


# ---- conditions ----

NEGATED = {"eq": "ne", "ne": "eq"}


def negate(condition):
    """The opposite condition, said directly where it can be."""
    if isinstance(condition, ir.Op):
        name, args = condition.name, condition.args
        if name in NEGATED:
            if name == "eq" and args[1] == ir.Const(0, 1) and args[0].bits == 1:
                return args[0]
            return ir.op(NEGATED[name], args, 1)
        if name == "lt":
            return ir.op("le", (args[1], args[0]), 1)
        if name == "le":
            return ir.op("lt", (args[1], args[0]), 1)
        if name == "ult":
            return ir.op("ule", (args[1], args[0]), 1)
        if name == "ule":
            return ir.op("ult", (args[1], args[0]), 1)
        if name == "lor":
            return ir.op("land", (negate(args[0]), negate(args[1])), 1)
        if name == "land":
            return ir.op("lor", (negate(args[0]), negate(args[1])), 1)
    return ir.op("eq", (condition, ir.Const(0, 1)), 1)


# ---- tidying the tree ----

def tidy(nodes, labels, in_loop=False):
    """Labels nothing jumps to go, loops get their conditions (while, do-while), a continue that ends a
    loop body goes, and so on."""
    out = []
    for node in nodes:
        if isinstance(node, Label) and node.label not in labels:
            continue
        if isinstance(node, If):
            node.then = tidy(node.then, labels, in_loop)
            node.otherwise = tidy(node.otherwise, labels, in_loop)
            if not node.then and node.otherwise:
                node = If(negate(node.condition), node.otherwise)
            if not node.then and not node.otherwise:
                continue
            # no else after a side that does not come back: "if (x) return; rest"
            if node.otherwise and ends_flow(node.then):
                out.append(If(node.condition, node.then))
                out += node.otherwise
                continue
            if node.otherwise and ends_flow(node.otherwise) and not ends_flow(node.then):
                out.append(If(negate(node.condition), node.otherwise))
                out += node.then
                continue
        elif isinstance(node, Loop):
            node.body = tidy(node.body, labels, True)
            node = loop_shape(node)
        elif isinstance(node, Switch):
            node.cases = [(values, tidy(body, labels, in_loop)) for values, body in node.cases]
        out.append(node)
    return out


def loop_shape(loop):
    body = loop.body
    # the body ends "if (c) continue; break;" or "if (c) continue;" followed by nothing: do { } while (c)
    while body and isinstance(body[-1], Continue):
        body.pop()
    # "if (c) continue; else break;" at the end: do { } while (c)
    if body and isinstance(body[-1], If) and len(body[-1].then) == 1 and len(body[-1].otherwise) == 1 \
            and not uses_continue(body[:-1]):
        last = body[-1]
        if isinstance(last.then[0], Continue) and isinstance(last.otherwise[0], Break):
            return Loop(body[:-1], "do", last.condition)
        if isinstance(last.then[0], Break) and isinstance(last.otherwise[0], Continue):
            return Loop(body[:-1], "do", negate(last.condition))
    if len(body) >= 2 and isinstance(body[-1], Break) and isinstance(body[-2], If) \
            and body[-2].then and len(body[-2].then) == 1 and isinstance(body[-2].then[0], Continue) \
            and not body[-2].otherwise and not uses_continue(body[:-2]):
        condition = body[-2].condition
        return Loop(body[:-2], "do", condition)
    if body and isinstance(body[-1], If) and not body[-1].otherwise and len(body[-1].then) == 1 \
            and isinstance(body[-1].then[0], Break) and not uses_continue(body[:-1]):
        return Loop(body[:-1], "do", negate(body[-1].condition))
    # the body starts "if (c) break;": while (!c) { }
    if body and isinstance(body[0], If) and not body[0].otherwise and len(body[0].then) == 1 \
            and isinstance(body[0].then[0], Break):
        return Loop(body[1:], "while", negate(body[0].condition))
    # the body ends "if (c) { ...; continue; } break;": the way out in the middle, "if (!c) break; ..."
    if len(body) >= 2 and isinstance(body[-1], Break) and isinstance(body[-2], If) and not body[-2].otherwise \
            and len(body[-2].then) >= 2 and isinstance(body[-2].then[-1], Continue):
        last = body[-2]
        return loop_shape(Loop(body[:-2] + [If(negate(last.condition), [Break()])] + last.then[:-1], "forever"))
    return Loop(body, "forever")


def uses_continue(nodes):
    for node in nodes:
        if isinstance(node, Continue):
            return True
        if isinstance(node, If) and (uses_continue(node.then) or uses_continue(node.otherwise)):
            return True
        if isinstance(node, Switch) and any(uses_continue(body) for _, body in node.cases):
            return True
    return False


def structure(function):
    """The tree for 'function', and the labels gotos still need."""
    structurer = Structurer(function)
    tree = tidy(structurer.build(), structurer.labels)
    fallthrough_cases(tree)
    drop_fallthrough_gotos(tree)
    labels = goto_targets(tree)
    return drop_labels(tree, labels), labels


def fallthrough_cases(nodes):
    """A case that is only a goto into the middle of another case, where that one's code runs on into
    it (the original lays case 3's code just before case 2's), goes right after that case, which then
    falls through into it the way C cases do: "case 3: ...; case 2: ...; break;"."""
    for node in nodes:
        if isinstance(node, If):
            fallthrough_cases(node.then)
            fallthrough_cases(node.otherwise)
        elif isinstance(node, Loop):
            fallthrough_cases(node.body)
        elif isinstance(node, Switch):
            for _, body in node.cases:
                fallthrough_cases(body)
            changed = True
            while changed:
                changed = False
                for b_index, (b_values, b_body) in enumerate(node.cases):
                    if len(b_body) != 1 or not isinstance(b_body[0], Goto):
                        continue
                    label = b_body[0].label
                    for a_index, (a_values, a_body) in enumerate(node.cases):
                        position = next((index for index, inner in enumerate(a_body)
                                         if isinstance(inner, Label) and inner.label == label), None)
                        if a_index == b_index or position is None:
                            continue
                        head, tail = a_body[:position], a_body[position:]
                        node.cases[a_index] = (a_values, head)
                        moved = (b_values, tail)
                        del node.cases[b_index]
                        node.cases.insert(node.cases.index((a_values, head)) + 1, moved)
                        changed = True
                        break
                    if changed:
                        break


def drop_labels(nodes, labels):
    """The tree without the labels no goto jumps to (and the ifs left with nothing to do)."""
    out = []
    for node in nodes:
        if isinstance(node, Label) and node.label not in labels:
            continue
        if isinstance(node, If):
            node.then = drop_labels(node.then, labels)
            node.otherwise = drop_labels(node.otherwise, labels)
            if not node.then and not node.otherwise:
                continue
            if not node.then:
                node = If(negate(node.condition), node.otherwise)
        elif isinstance(node, Loop):
            node.body = drop_labels(node.body, labels)
        elif isinstance(node, Switch):
            node.cases = [(values, drop_labels(body, labels)) for values, body in node.cases]
        out.append(node)
    return out


def drop_fallthrough_gotos(nodes, follow=frozenset()):
    """A goto that ends a branch, to a label the code reaches anyway when the branch ends (the label just
    after the if), goes. 'follow': the labels falling off the end of 'nodes' reaches."""
    for position, node in enumerate(nodes):
        after = set()
        rest = position + 1
        while rest < len(nodes) and isinstance(nodes[rest], Label):
            after.add(nodes[rest].label)
            rest += 1
        if rest == len(nodes):
            after |= follow
        if isinstance(node, If):
            drop_fallthrough_gotos(node.then, frozenset(after))
            drop_fallthrough_gotos(node.otherwise, frozenset(after))
        elif isinstance(node, Loop):
            drop_fallthrough_gotos(node.body)
        elif isinstance(node, Switch):
            for _, body in node.cases:
                drop_fallthrough_gotos(body)
    if nodes and isinstance(nodes[-1], Goto) and nodes[-1].label in follow:
        nodes.pop()


def goto_targets(nodes):
    """The labels the gotos of a tree jump to."""
    found = set()
    for node in nodes:
        if isinstance(node, Goto):
            found.add(node.label)
        elif isinstance(node, If):
            found |= goto_targets(node.then) | goto_targets(node.otherwise)
        elif isinstance(node, Loop):
            found |= goto_targets(node.body)
        elif isinstance(node, Switch):
            for _, body in node.cases:
                found |= goto_targets(body)
    return found
