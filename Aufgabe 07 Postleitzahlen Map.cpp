// AufgabePostleitzahlen.cpp : Diese Datei enth�lt die Funktion "main". Hier beginnt und endet die Ausf�hrung des Programms.
//

#include <iostream>
#include <sstream>
#include <fstream>

#include <map>
#include <vector>

// F�r die Sortierung
#include <algorithm>

using namespace std;

map<string, vector<string>> PLZMap;

void postleitzahlenDateiEinlesen();
bool leseDatei(const string&);
void postleitzahlSuchen();
void ausgabeAllerPostleitzahlen();
void ortSuchen();
void ausgabeAllerOrte();

vector<string> stringSplit(const string&, const char);
string toLower(const string&);
string replaceAll(string, const string&, const string&);
int subStringCount(const string&, const string&);


int main()
{
	// F�r die Ausgabe von Programmtexten mit Umlauten auf der Konsole
	locale::global(locale("de_DE.utf-8"));

	postleitzahlenDateiEinlesen();

}

void postleitzahlenDateiEinlesen()
{
	string eingabe;

	cout << "\nName der Postleitzahlendatei: " << endl;
	getline(cin, eingabe);

	if (!leseDatei(eingabe))
		return;

	cout << "\nSoll nach einer Postleitzahl gesucht werden (j/n): " << endl;
	getline(cin, eingabe);

	if (toLower(eingabe) == "j")
		postleitzahlSuchen();

	cout << "\nSoll nach einem Ort gesucht werden (j/n): " << endl;
	getline(cin, eingabe);

	if (toLower(eingabe) == "j")
		ortSuchen();

	cout << "\nSoll eine Liste aller Postleitzahlen �ber die Konsole ausgegeben werden (j/n): " << endl;
	getline(cin, eingabe);

	if (toLower(eingabe) == "j")
		ausgabeAllerPostleitzahlen();

	cout << "\nSoll eine Liste aller Orte aufsteigend sortiert �ber die Konsole ausgegeben werden (j/n): " << endl;
	getline(cin, eingabe);

	if (toLower(eingabe) == "j")
		ausgabeAllerOrte();

}

bool leseDatei(const string& dateiname)
{
	string zeile, sOrt;
	int recordCount = 0;
	int addCount = 0;

	cout << "Die Postleitzahlendatei '" << dateiname << "' wird eingelesen..." << endl;

	ifstream datei(dateiname);

	if (datei.fail())
	{
		cerr << "Fehler beim �ffnen der Datei '" << dateiname << "'" << endl;
		return false;
	}

	while (!datei.eof())
	{
		getline(datei, zeile);

		recordCount++;

		// Die Ausgabe von Texten �ber die Konsole kostet Zeit.
		// Deshalb soll nur nach jedem 50. Satz die Ausgabe �ber die Konsole aktualsiert werden.
		// Danach wird der Cursor wieder an den Anfang der Zeile zur�ckgesetzt.
		// Die folgende Ausgabe �berschreibt dann die Werte in der aktuellen Zeile.
		if (recordCount % 50 == 0)
			cout << "[" << recordCount << "]\r";

		// Aufteilen der Zeile in Postleitzahl und Ort
		vector<string> v = stringSplit(zeile, ';');

		if (v.size() >= 2)
		{
			// Manche Orte enthalten zus�tzlich ein Semikolon und danach noch einen Ortsteil
			sOrt = v[1];

			for (int i = 2; i < v.size(); i++)
				sOrt += "," + v[i];


			PLZMap[v[0]].push_back(sOrt);
			addCount++;
		}

	}

	cout << "[" << recordCount << "]";

	datei.close();

	printf("\n\nEs wurden %d Datens�tze erfolgreich eingelesen.\n", recordCount);
	printf("Es wurden %d Eintr�ge zur Tabelle der Postleitzahlen hinzugef�gt.\n", addCount);
	printf("Die Tabelle der Postleitzahlen enth�lt %lld Eintr�ge.\n", PLZMap.size());

	return true;
}

void postleitzahlSuchen()
{

	vector<string> vOrte;
	string eingabe;

	// Konsole l�schen
	system("cls");

	while (true)
	{

		cout << "\nPostleitzahl eingeben oder 'exit' um die Suche zu beenden" << endl;
		getline(cin, eingabe);

		if (eingabe.length() == 0)
			continue;

		// Gross-/Kleinschreibung ist nicht relevant
		if (toLower(eingabe) == "exit")
			break;

		if (PLZMap.find(eingabe) != PLZMap.end())
		{
			vOrte = PLZMap[eingabe];

			// Aufsteigende Sortierung der Orte
			sort(vOrte.begin(), vOrte.end());

			for (const auto& eintrag : vOrte)
			{
				cout << eintrag << endl;
			}

			cout << "\n\n***   Ende der Liste   ***" << endl;
			cout << "Es wurden " << vOrte.size() << " Eintr�ge gefunden" << endl;
			cout << endl;

		}
		else
			cout << "Die Postleitzahl wurde nicht gefunden." << endl;
	}
}

void ausgabeAllerPostleitzahlen()
{
	vector<string> vOrte;
	int entryCount = 0;

	// Konsole l�schen
	system("cls");

	cout << "Liste aller Eintr�ge in der Tabelle Postleitzahlen" << endl;
	cout << "--------------------------------------------------" << endl;

	// Jedes einzelne Schl�ssel-Werte-Paar (pair) zur�ckgeliefert
	for (const auto& eintrag : PLZMap)
	{
		vOrte = eintrag.second;

		// Aufsteigende Sortierung der Orte
		sort(vOrte.begin(), vOrte.end());

		for (const auto& ort : vOrte)
		{
			cout << eintrag.first << " - " << ort << endl;
			entryCount++;
		}

	}

	cout << "\n\n***   Ende der Liste   ***" << endl;
	cout << "Es wurden " << entryCount << " Eintr�ge gefunden." << endl;
	cout << endl;

}

void ortSuchen()
{
	int entryCount = 0;
	string eingabe;
	string suchOrt;
	string aktOrt;
	bool found = false;
	size_t findPos;


	while (true)
	{
		cout << "\nVollst�ndigen Ort, *suchbegriff, suchbegriff*, *suchbegriff* oder 'exit' eingeben: " << endl;
		getline(cin, eingabe);

		if (eingabe.length() == 0)
			continue;

		if (toLower(eingabe) == "exit")
			break;

		// Nur maximal 2 Wildcards d�rfen eingegeben werden
		if (subStringCount(eingabe, "*") > 2)
		{
			cerr << "Suchbegriff ung�ltig" << endl;
			continue;
		}

		// Keine unmittelbar aufeinander folgenden Wildcards 
		if (eingabe.find("**") != string::npos)
		{
			cerr << "Suchbegriff ung�ltig" << endl;
			continue;
		}

		// Konsole l�schen
		system("cls");

		// Ser suchOrt enth�lt den Suchbegriff ohne Wildcards (*) in Kleinbuchstaben.
		suchOrt = toLower(replaceAll(eingabe, "*", ""));

		entryCount = 0;

		for (const auto& eintrag : PLZMap)
		{
			// eintrag.first ist der Schl�ssel (Postleitzahl)
			// eintrag.second ist der Wert (vector<string>)

			vector<string> vOrte = eintrag.second;

			// Lesen aller Eintr�ge des Vectors

			for (const auto& orgOrt : vOrte)
			{
				aktOrt = toLower(orgOrt);

				// Wenn der Suchbegriff nicht im aktuelle Ort enthalten ist,
				// n�chsten Ort lesen.
				if (aktOrt.find(suchOrt) == string::npos)
					continue;

				found = false;

				// *suchbegriff* : Suchbegriff kann irgendwo im Ort enthalten sein
				if (eingabe[0] == '*' && eingabe[eingabe.length() - 1] == '*')
				{
					found = (aktOrt.find(suchOrt) != string::npos);
				}
				// *suchbegriff : Der Ort mu� mit dem Suchbegriff enden
				else if (eingabe[0] == '*')
				{

					// rfind() durchsucht die Zeichenkette von rechts nach links
					findPos = aktOrt.rfind(suchOrt);
					if (findPos != string::npos)
						// Der Suchbegriff mu� sich am Ende des Orts befinden
						found = (findPos == aktOrt.length() - suchOrt.length());
				}
				// suchbegriff* : Der Ort mu� mit dem Suchbegriff beginnen
				else if (eingabe[eingabe.length() - 1] == '*')
				{
					found = (aktOrt.find(suchOrt) == 0);
				}
				// suchbegriff : Der Ort mu� so hei�en wie der angegebene Suchbegriff
				else
				{
					found = (aktOrt == suchOrt);
				}

				if (found)
				{
					cout << eintrag.first << " - " << orgOrt << endl;
					entryCount++;
				}
			}
		}

		cout << "\n\n***   Ende der Liste   ***" << endl;
		cout << "Es wurden " << entryCount << " Eintr�ge gefunden" << endl;
		cout << endl;
	}
}

void ausgabeAllerOrte()
{
	int entryCount = 0;
	string eingabe;

	// F�r die Zeitmessung
	clock_t start, ende;
	double sekunden;

	// Vector f�r alle Orte auf dem Heap erstellen
	vector<string>* vAlleOrte = new vector<string>();

	// Konsole l�schen
	system("cls");

	cout << "Liste aller Eintr�ge in der Tabelle Postleitzahlen " << endl;
	cout << "aufsteigend sortiert nach Ort" << endl;
	cout << "---------------------------------------------------" << endl;

	cout << "Datens�tze werden gelesen...\n";

	start = clock();

	for (const auto& eintrag : PLZMap)
	{
		vector<string> vOrte = eintrag.second;

		//vAlleOrte->insert(vAlleOrte->end(), vOrte.begin(), vOrte.end());

		for (const auto& s : vOrte)
		{
			vAlleOrte->push_back(s + "#" + eintrag.first);
		}
	}

	ende = clock();

	// F�r die Genauigkeit Umwandlung in double
	sekunden = (ende - start) / static_cast<double>(CLOCKS_PER_SEC);
	printf("Dauer: %1.2f Sekunden\n", sekunden);

	cout << "Weiter mit der Eingabetaste...\n" << endl;
	getline(cin, eingabe);

	cout << "Datens�tze werden sortiert...\n" << endl;

	start = clock();

	sort(vAlleOrte->begin(), vAlleOrte->end());

	ende = clock();

	// F�r die Genauigkeit Umwandlung in double
	sekunden = (ende - start) / static_cast<double>(CLOCKS_PER_SEC);
	printf("Dauer: %1.2f Sekunden\n", sekunden);

	cout << "Weiter mit der Eingabetaste...\n" << endl;
	getline(cin, eingabe);

	for (const auto& ort : *vAlleOrte)
	{

		// Trennen von Ort und Postleitzahle durch das selbst definierte Trennzeichen
		vector<string> v = stringSplit(ort, '#');

		if (v.size() >= 2)
			cout << v[1] + " - " << v[0] << endl;
		else
			cout << ort << endl;

		entryCount++;
	}

	// Den Vector vom Heap l�schen
	delete vAlleOrte;

	cout << "\n\n***   Ende der Liste   ***" << endl;
	cout << "Es wurden " << entryCount << " Eintr�ge gefunden" << endl;
	cout << endl;

}

vector<string> stringSplit(const string& s, const char delim)
{
	vector<string> v;

	stringstream sstream(s);
	string element;

	// getline liest die Zeichen aus dem stringstream bis zum jeweiligen Trennzeichen (Delimiter) 
	// in die Zeichenkette 'element'. 
	// Wenn alle Zeichen eingelesen wurden, ist der R�ckgabewert von getline() 'false'.
	while (getline(sstream, element, delim))
	{
		v.push_back(element);
	}

	return v;
}

string toLower(const string& s)
{
	string retValue;

	for (int i = 0; i < s.length(); i++)
	{
		retValue += tolower(s.at(i));
	}

	return retValue;
}

string replaceAll(string s, const string& old, const string& replace)
{

	size_t position = 0;

	do
	{
		position = s.find(old, position);

		if (position != string::npos)
		{
			s.replace(position, old.length(), replace);
			position += replace.length();
		}
	} while (position != string::npos);

	return s;
}

int subStringCount(const string& s, const string& subString)
{

	int retValue = 0;

	// Muss gepr�ft werden, sonst Endlosschleife (subString.length() = 0)
	// Nichts zu suchen
	if (subString.length() == 0)
		return retValue;

	// L�nge der Basiszeichenkette abz�glich der L�nge der Zeichenkette
	// in der alle Suchbegriffe entfernt wurden, dividiert durch die L�nge
	// des Suchbegriffs.
	return (s.length() - replaceAll(s, subString, "").length()) / subString.length();

}

// Programm ausf�hren: STRG+F5 oder Men�eintrag "Debuggen" > "Starten ohne Debuggen starten"
// Programm debuggen: F5 oder "Debuggen" > Men� "Debuggen starten"

// Tipps f�r den Einstieg: 
//   1. Verwenden Sie das Projektmappen-Explorer-Fenster zum Hinzuf�gen/Verwalten von Dateien.
//   2. Verwenden Sie das Team Explorer-Fenster zum Herstellen einer Verbindung mit der Quellcodeverwaltung.
//   3. Verwenden Sie das Ausgabefenster, um die Buildausgabe und andere Nachrichten anzuzeigen.
//   4. Verwenden Sie das Fenster "Fehlerliste", um Fehler anzuzeigen.
//   5. Wechseln Sie zu "Projekt" > "Neues Element hinzuf�gen", um neue Codedateien zu erstellen, bzw. zu "Projekt" > "Vorhandenes Element hinzuf�gen", um dem Projekt vorhandene Codedateien hinzuzuf�gen.
//   6. Um dieses Projekt sp�ter erneut zu �ffnen, wechseln Sie zu "Datei" > "�ffnen" > "Projekt", und w�hlen Sie die SLN-Datei aus.
