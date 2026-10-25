/**
 *    Name:        folder_dialog.c
 *    Description: the system's dialog to choose a folder: on Windows COM's IFileOpenDialog for folders, on
 *                 macOS AppleScript's choose folder through osascript, on Linux zenity's or else kdialog's
 *    Version:     0.1.0
 *    Created:     25-10-2026
 *    Modified:    25-10-2026
 *    Author:      Sebastian Toyos
 *
 *    license:BSD-3-Clause
 *    On macOS and Linux the dialog is another program, run through popen: it writes the folder chosen on
 *    its output and ends with 0, or with another code when it is cancelled; a shell that cannot find it ends
 *    with 127, and then the next is tried. Their commands are fixed text: nothing of the player's goes into
 *    them. On Windows the folder comes in UTF-16 and goes to the C library in the system's code page, as
 *    the tool's other paths do.
 */
#include "folder_dialog.h"

#include <stdio.h>
#include <string.h>

#define FOLDER_TITLE "The folder for SegaSonic.nsp"

#ifdef _WIN32

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600    /* (Vista: the dialog's interface) */
#endif
#define COBJMACROS
#include <windows.h>
#include <shobjidl.h>

int folder_dialog(void *owner, const char *start, char *folder, size_t size)
{
	const HRESULT com = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	IFileOpenDialog *dialog = NULL;
	IShellItem *item = NULL;
	FILEOPENDIALOGOPTIONS options;
	int got = FOLDER_CANCELLED;

	if (FAILED(com) && com != RPC_E_CHANGED_MODE)
		return FOLDER_NO_DIALOG;
	if (FAILED(CoCreateInstance(&CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, &IID_IFileOpenDialog,
		(void **)&dialog)))
	{
		if (SUCCEEDED(com))
			CoUninitialize();
		return FOLDER_NO_DIALOG;
	}
	if (SUCCEEDED(IFileOpenDialog_GetOptions(dialog, &options)))
		IFileOpenDialog_SetOptions(dialog, options | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM | FOS_PATHMUSTEXIST);
	IFileOpenDialog_SetTitle(dialog, L"" FOLDER_TITLE);
	if (start && start[0])
	{
		WCHAR wide[MAX_PATH * 4];
		IShellItem *first = NULL;

		if (MultiByteToWideChar(CP_ACP, 0, start, -1, wide, MAX_PATH * 4) > 0
			&& SUCCEEDED(SHCreateItemFromParsingName(wide, NULL, &IID_IShellItem, (void **)&first)))
		{
			IFileOpenDialog_SetFolder(dialog, first);
			IShellItem_Release(first);
		}
	}
	if (SUCCEEDED(IFileOpenDialog_Show(dialog, (HWND)owner)) && SUCCEEDED(IFileOpenDialog_GetResult(dialog, &item)))
	{
		PWSTR chosen = NULL;

		if (SUCCEEDED(IShellItem_GetDisplayName(item, SIGDN_FILESYSPATH, &chosen)))
		{
			BOOL lossy = FALSE;

			/* (a name the code page has no letters for cannot be opened with fopen) */
			got = WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, chosen, -1, folder, (int)size, NULL, &lossy) > 0
				&& !lossy ? FOLDER_CHOSEN : FOLDER_UNUSABLE;
			CoTaskMemFree(chosen);
		}
		IShellItem_Release(item);
	}
	IFileOpenDialog_Release(dialog);
	if (SUCCEEDED(com))
		CoUninitialize();
	return got;
}

#else

#include <sys/wait.h>

/* One dialog program: what it wrote, or FOLDER_NO_DIALOG when the shell did not find it */
static int run_dialog(const char *command, char *folder, size_t size)
{
	FILE *dialog = popen(command, "r");
	char line[4096] = "";
	int status;
	size_t length;

	if (!dialog)
		return FOLDER_NO_DIALOG;
	if (!fgets(line, sizeof(line), dialog))
		line[0] = '\0';
	while (fgetc(dialog) != EOF)
		;    /* (what is left, so that it ends) */
	status = pclose(dialog);
	if (status == -1 || (WIFEXITED(status) && WEXITSTATUS(status) == 127))
		return FOLDER_NO_DIALOG;
	if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
		return FOLDER_CANCELLED;
	length = strlen(line);
	while (length > 0 && (line[length - 1] == '\n' || line[length - 1] == '\r'))
		line[--length] = '\0';
	if (!length)
		return FOLDER_CANCELLED;
	if (length >= size)
		return FOLDER_UNUSABLE;
	memcpy(folder, line, length + 1);
	return FOLDER_CHOSEN;
}

int folder_dialog(void *owner, const char *start, char *folder, size_t size)
{
	(void)owner;
	(void)start;
#ifdef __APPLE__
	return run_dialog("osascript -e 'POSIX path of (choose folder with prompt \"" FOLDER_TITLE "\")' 2>/dev/null",
		folder, size);
#else
	{
		const int got = run_dialog("zenity --file-selection --directory --title='" FOLDER_TITLE "' 2>/dev/null",
			folder, size);

		if (got != FOLDER_NO_DIALOG)
			return got;
		return run_dialog("kdialog --getexistingdirectory . --title '" FOLDER_TITLE "' 2>/dev/null", folder, size);
	}
#endif
}

#endif
