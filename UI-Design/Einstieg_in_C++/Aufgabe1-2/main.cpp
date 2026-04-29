#include <iostream>
#include <string>

/**
 * Autor: Henry Mirus
 * Eingabeparameter: argc - Anzahl der Programmargumente; argv - Argumentliste.
 * Rueckgabeparameter: 0 bei Erfolg, sonst 1 bei fehlerhafter Eingabe.
 * Seiteneffekte: Liest Namen von der Kommandozeile und schreibt Text in die Konsole.
 */
int main(int argc, char* argv[])
{
	if (argc < 2) {
		std::cerr << "Fehler: Kein Name angegeben.\n";
		std::cerr << "Verwendung: " << argv[0] << " <Name>\n";
		return 1;
	}

	if (argc > 2) {
		std::cerr << "Fehler: Zu viele Argumente.\n";
		std::cerr << "Verwendung: " << argv[0] << " <Name>\n";
		return 1;
	}

	std::string name = argv[1];

	if (name.empty()) {
		std::cerr << "Fehler: Name ist leer.\n";
		return 1;
	}

	if (name.find_first_not_of(" \t\n\r") == std::string::npos) {
		std::cerr << "Fehler: Name darf nicht nur aus Leerzeichen bestehen.\n";
		return 1;
	}

	if (name.length() > 100) {
		std::cerr << "Fehler: Name ist zu lang (Maximum: 100 Zeichen).\n";
		return 1;
	}

	std::cout << "Hallo " << name << ",\n";
	std::cout << "Willkommen in der Vorlesung User Interface Design!\n";
	return 0;
}