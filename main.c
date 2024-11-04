#include <stdio.h>
#include <windows.h>
#include <string.h>

//persistence.exe hklm MeinProgramm "C:\\Pfad\\zu\\programm.exe" --option1 --option2


void set_persistence(const char* path, const char* args, const char* valueName, BOOL useHKLM) {
    HKEY hKey;
    LONG result;
    char fullPath[MAX_PATH + 256];

    // Den vollständigen Pfad inklusive Argumente erstellen
    snprintf(fullPath, sizeof(fullPath), "\"%s\" %s", path, args);

    // Bestimme Registry-Schlüssel basierend auf der Auswahl des Benutzers
    HKEY rootKey = useHKLM ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER;
    const char* regPath = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";

    // Öffne oder erstelle den Registry-Schlüssel
    result = RegOpenKeyExA(rootKey, regPath, 0, KEY_WRITE, &hKey);
    if (result != ERROR_SUCCESS) {
        printf("Fehler beim Öffnen des Schlüssels: %ld\n", result);
        return;
    }

    // Setze den Eintrag für Persistenz
    result = RegSetValueExA(hKey, valueName, 0, REG_SZ, (BYTE*)fullPath, strlen(fullPath) + 1);
    if (result == ERROR_SUCCESS) {
        printf("Persistenz erfolgreich gesetzt in %s\\%s\n", useHKLM ? "HKLM" : "HKCU", regPath);
    } else {
        printf("Fehler beim Setzen des Werts: %ld\n", result);
    }

    // Schließe den Schlüssel
    RegCloseKey(hKey);
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        printf("Verwendung: %s <hklm|hkcu> <name> <exe> [parameter]\n", argv[0]);
        return 1;
    }

    BOOL useHKLM = _stricmp(argv[1], "hklm") == 0; // Vergleicht Argument mit "hklm" (Groß-/Kleinschreibung ignoriert)
    const char* valueName = argv[2]; // Der Name des Persistenz-Eintrags
    const char* exePath = argv[3];
    char args[256] = "";

    // Sammle optionale Parameter für die Ausführung
    for (int i = 4; i < argc; i++) {
        strcat(args, argv[i]);
        strcat(args, " ");
    }

    // Persistenz setzen
    set_persistence(exePath, args, valueName, useHKLM);

    return 0;
}
