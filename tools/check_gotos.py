"""
Name:        check_gotos.py
Description: checks that taking the gotos out of the C (decompiler/gotos.py) keeps what each function runs
Version:     0.1.0
Created:     16-10-2026
Modified:    16-10-2026
Author:      Sebastian Toyos

Decompiles the game (and with --sound the sound program) into a scratch folder and, for every function whose
tree had gotos, runs the tree as the structurer left it and the tree without gotos on the same made-up
outcomes: the n-th time a condition is tested it comes out the same in both (a test and its opposite, "a <
b" and "b <= a", agree). Both must run the same blocks in the same order and end the same way. The code a
scenario never reaches (the unused video routines) is checked this way too.

Usage: python tools/check_gotos.py OUT_DIR [--sound] [--runs N]
"""
import copy
import random
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(Path(__file__).resolve().parent))

from decompiler import gotos  # noqa: E402
from decompiler import structure as tree_nodes  # noqa: E402
from v60 import ir  # noqa: E402

STEPS = 3000        # blocks a run may go through
ROUNDS = 20000      # loop rounds a run may go through


class Leave(Exception):
    pass


class BreakOut(Exception):
    pass


class GoRound(Exception):
    pass


class Jump(Exception):
    def __init__(self, label):
        super().__init__(label)
        self.label = label


class Stop(Exception):
    pass


class Runner:
    """Runs a tree: conditions come from 'outcomes', blocks and ends go into 'trace'."""

    def __init__(self, outcomes):
        self.outcomes = outcomes
        self.counts = {}
        self.trace = []
        self.controls = {}
        self.steps = 0
        self.rounds = 0

    def run(self, tree):
        try:
            self.nodes(tree)
            self.trace.append(("off the end",))
        except Leave:
            pass
        except Stop as stop:
            self.trace.append(("stopped", str(stop)))
        except Jump as jump:
            self.trace.append(("goto nowhere", jump.label))
        except (BreakOut, GoRound):
            self.trace.append(("break or continue outside a loop",))
        return self.trace

    # ---- conditions ----

    def outcome(self, key):
        count = self.counts.get(key, 0)
        self.counts[key] = count + 1
        return self.outcomes(key, count)

    def test(self, condition):
        if isinstance(condition, ir.Control):
            return self.controls.get(condition.name, False)
        if isinstance(condition, ir.Const):
            return condition.value != 0
        if isinstance(condition, ir.Op):
            name, args = condition.name, condition.args
            if name == "lor":
                return self.test(args[0]) or self.test(args[1])
            if name == "land":
                return self.test(args[0]) and self.test(args[1])
            if name in ("eq", "ne") and args[1] == ir.Const(0, 1) and args[0].bits == 1:
                value = self.test(args[0])
                return not value if name == "eq" else value
            if name == "lt":
                return self.outcome(("lt", args[0], args[1]))
            if name == "le":
                return not self.outcome(("lt", args[1], args[0]))
            if name == "ult":
                return self.outcome(("ult", args[0], args[1]))
            if name == "ule":
                return not self.outcome(("ult", args[1], args[0]))
            if name == "eq":
                return self.outcome(("eq", args[0], args[1]))
            if name == "ne":
                return not self.outcome(("eq", args[0], args[1]))
        return self.outcome(("value", condition))

    # ---- the tree ----

    def nodes(self, nodes, start=None):
        index, entering = 0, start
        if entering is not None:
            index = position(nodes, entering)
        while index < len(nodes):
            node = nodes[index]
            try:
                if entering is not None and not (isinstance(node, tree_nodes.Label) and node.label == entering):
                    target, entering = entering, None
                    self.enter(node, target)
                else:
                    entering = None
                    self.node(node)
            except Jump as jump:
                found = position(nodes, jump.label)
                if found is None:
                    raise
                index, entering = found, jump.label
                continue
            index += 1

    def node(self, node):
        if isinstance(node, tree_nodes.Code):
            self.steps += 1
            if self.steps > STEPS:
                raise Stop("steps")
            self.trace.append(("block", node.label))
        elif isinstance(node, tree_nodes.Mark):
            self.controls[node.name] = self.test(node.value)
        elif isinstance(node, tree_nodes.End):
            end = node.end
            self.trace.append(("end", type(end).__name__, getattr(end, "target", None)))
            raise Leave()
        elif isinstance(node, tree_nodes.Break):
            raise BreakOut()
        elif isinstance(node, tree_nodes.Continue):
            raise GoRound()
        elif isinstance(node, tree_nodes.Goto):
            raise Jump(node.label)
        elif isinstance(node, tree_nodes.If):
            self.nodes(node.then if self.test(node.condition) else node.otherwise)
        elif isinstance(node, tree_nodes.Loop):
            self.loop(node, None)
        elif isinstance(node, tree_nodes.Switch):
            self.switch(node, None)

    def enter(self, node, label):
        """Runs 'node' from 'label', somewhere inside it."""
        if isinstance(node, tree_nodes.If):
            self.nodes(node.then if position(node.then, label) is not None else node.otherwise, label)
        elif isinstance(node, tree_nodes.Loop):
            self.loop(node, label)
        elif isinstance(node, tree_nodes.Switch):
            self.switch(node, label)
        else:
            raise Stop(f"cannot enter {type(node).__name__}")

    def loop(self, loop, start):
        first = True
        while True:
            self.rounds += 1
            if self.rounds > ROUNDS:
                raise Stop("rounds")
            if loop.kind == "while" and not (first and start is not None) and not self.test(loop.condition):
                return
            try:
                self.nodes(loop.body, start if first else None)
            except BreakOut:
                return
            except GoRound:
                pass
            first = False
            if loop.kind == "do" and not self.test(loop.condition):
                return

    def switch(self, switch, start):
        bodies = [body for _, body in switch.cases]
        if start is not None:
            chosen = next(index for index, body in enumerate(bodies) if position(body, start) is not None)
        else:
            chosen = self.outcome(("switch", switch.value)) % len(bodies)
        try:
            for index in range(chosen, len(bodies)):
                self.nodes(bodies[index], start if index == chosen else None)
        except BreakOut:
            pass


def position(nodes, label):
    """The index in 'nodes' of the label, or of the node it is inside; None."""
    for index, node in enumerate(nodes):
        if isinstance(node, tree_nodes.Label) and node.label == label:
            return index
        if any(position(inner, label) is not None for inner in gotos.children(node)):
            return index
    return None


def outcomes_for(seed):
    """The made-up outcomes of one run: the n-th test of a condition, the same in both trees."""
    cache = {}

    def outcome(key, count):
        found = cache.get((key, count))
        if found is None:
            # (hash() is the same for equal keys within one process: both trees run here)
            value = random.Random(hash((seed, key, count))).getrandbits(16)
            found = value if key[0] == "switch" else bool(value & 1)
            cache[(key, count)] = found
        return found
    return outcome


def compare(name, before, after, runs):
    """Problems found running the two trees 'runs' times."""
    problems = []
    for seed in range(runs):
        first = Runner(outcomes_for(seed)).run(before)
        second = Runner(outcomes_for(seed)).run(after)
        if first != second:
            at = next((index for index, (a, b) in enumerate(zip(first, second)) if a != b), min(len(first), len(second)))
            problems.append(f"{name}: run {seed} differs at step {at}: {first[at:at + 3]} / {second[at:at + 3]}")
            break
    return problems


def main():
    arguments = sys.argv[1:]
    if not arguments:
        raise SystemExit(__doc__)
    out_dir = Path(arguments[0])
    runs = int(arguments[arguments.index("--runs") + 1]) if "--runs" in arguments else 300
    checked, problems = [], []
    original = gotos.eliminate

    def checking(tree, control_name):
        before = copy.deepcopy(tree)
        after = original(tree, control_name)
        name = control_name.__self__.function.name
        checked.append(name)
        problems.extend(compare(name, before, after, runs))
        left = [node for node in gotos.walk(after) if isinstance(node, tree_nodes.Goto)]
        if left:
            problems.append(f"{name}: {len(left)} gotos left")
        return after
    gotos.eliminate = checking
    if "--sound" in arguments:
        import decompile_sound
        sys.argv = ["decompile_sound.py", "--out", str(out_dir)]
        decompile_sound.main()
    else:
        import decompile
        sys.argv = ["decompile.py", "--out", str(out_dir)]
        decompile.main()
    print(f"{len(checked)} functions had gotos; {len(problems)} problems")
    for problem in problems:
        print("  " + problem)
    return 1 if problems else 0


if __name__ == "__main__":
    sys.exit(main())
