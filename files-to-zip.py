#!/usr/bin/env python3
"""
files-to-zip.py - Packt mehrere Dateien in eine ZIP-Datei
Verwendung: python files-to-zip.py <datei1> [<datei2> <datei3> ...]
Die ZIP wird im Verzeichnis der ersten Datei mit dem Default-Namen 'archive.zip' erstellt.
"""

import argparse
import sys
import os
import zipfile
from pathlib import Path


class FilesToZip:
    def __init__(self, files, output_name='archive.zip'):
        """
        Initialisiert den Zipper.
        
        Args:
            files: Liste von Dateipfaden
            output_name: Name der ZIP-Datei (default: archive.zip)
        """
        self.files = [Path(f) for f in files]
        self.output_name = output_name
        
        # Validierung
        if not self.files:
            raise ValueError("Mindestens eine Datei muss angegeben werden")
        
        # Alle Dateien müssen existieren
        for file_path in self.files:
            if not file_path.exists():
                raise FileNotFoundError(f"Datei nicht gefunden: {file_path}")
            if not file_path.is_file():
                raise ValueError(f"Ist keine Datei: {file_path}")
        
        # Zielverzeichnis: Verzeichnis der ersten Datei
        self.output_dir = self.files[0].parent
        self.output_path = self.output_dir / output_name
    
    def create_zip(self, verbose=False):
        """
        Erstellt die ZIP-Datei.
        
        Args:
            verbose: Wenn True, gibt Informationen über jede eingefügte Datei aus
        """
        try:
            with zipfile.ZipFile(self.output_path, 'w', zipfile.ZIP_DEFLATED) as zipf:
                for file_path in self.files:
                    # Arcname: Pfad in der ZIP (nur Dateiname für Übersichtlichkeit)
                    arcname = file_path.name
                    zipf.write(file_path, arcname=arcname)
                    
                    if verbose:
                        file_size = file_path.stat().st_size
                        print(f"  + {file_path.name} ({file_size:,} bytes)")
            
            # Größe der ZIP-Datei
            zip_size = self.output_path.stat().st_size
            file_count = len(self.files)
            
            print(f"✓ ZIP-Archiv erfolgreich erstellt: {self.output_path}")
            print(f"  Dateien: {file_count}")
            print(f"  Größe: {zip_size:,} bytes ({zip_size / 1024 / 1024:.2f} MB)")
            
        except Exception as e:
            print(f"✗ Fehler beim Erstellen der ZIP-Datei: {e}", file=sys.stderr)
            raise


def main():
    parser = argparse.ArgumentParser(
        description='Packt mehrere Dateien in eine ZIP-Datei',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog='Beispiele:\n'
               '  python files-to-zip.py file1.txt file2.pdf file3.csv\n'
               '  python files-to-zip.py -o backup.zip *.py\n'
               '  python files-to-zip.py -v file1.txt file2.txt'
    )
    
    parser.add_argument(
        'files',
        nargs='+',
        help='Dateien zum Packen (mindestens eine)'
    )
    
    parser.add_argument(
        '-o', '--output',
        default='archive.zip',
        help='Name der ZIP-Datei (default: archive.zip)'
    )
    
    parser.add_argument(
        '-v', '--verbose',
        action='store_true',
        help='Detaillierte Ausgabe mit jeder eingefügten Datei'
    )
    
    args = parser.parse_args()
    
    try:
        zipper = FilesToZip(args.files, args.output)
        zipper.create_zip(verbose=args.verbose)
    except FileNotFoundError as e:
        print(f"Fehler: {e}", file=sys.stderr)
        sys.exit(1)
    except ValueError as e:
        print(f"Fehler: {e}", file=sys.stderr)
        sys.exit(1)
    except Exception as e:
        print(f"Fehler beim Packen: {e}", file=sys.stderr)
        sys.exit(1)


if __name__ == '__main__':
    main()
