#!/usr/bin/env python3
"""
md-to-pdf.py - Konvertiert Markdown-Dateien in PDF-Dateien
Verwendung: python md-to-pdf.py <pfad-zur-md-datei>
Die PDF wird im selben Ordner mit gleichem Namen erstellt.
"""

import argparse
import sys
import os
from pathlib import Path
from reportlab.lib.pagesizes import A4
from reportlab.lib.styles import getSampleStyleSheet, ParagraphStyle
from reportlab.lib.units import cm
from reportlab.platypus import SimpleDocTemplate, Paragraph, Spacer, PageBreak, Table, TableStyle
from reportlab.lib import colors
from reportlab.lib.enums import TA_LEFT, TA_JUSTIFY


class MarkdownToPDF:
    def __init__(self, md_file):
        self.md_file = Path(md_file)
        if not self.md_file.exists():
            raise FileNotFoundError(f"Markdown-Datei nicht gefunden: {md_file}")
        if self.md_file.suffix != '.md':
            raise ValueError(f"Datei muss eine .md-Datei sein: {md_file}")
        
        self.pdf_file = self.md_file.with_suffix('.pdf')
        self.styles = self._create_styles()
        
    def _create_styles(self):
        """Erstellt benutzerdefinierte Stile für PDF."""
        styles = getSampleStyleSheet()
        
        # Titel (H1)
        styles.add(ParagraphStyle(
            name='CustomH1',
            parent=styles['Heading1'],
            fontSize=24,
            textColor=colors.HexColor('#1a1a1a'),
            spaceAfter=12,
            spaceBefore=12,
            fontName='Helvetica-Bold'
        ))
        
        # Überschrift 2 (H2)
        styles.add(ParagraphStyle(
            name='CustomH2',
            parent=styles['Heading2'],
            fontSize=16,
            textColor=colors.HexColor('#2c3e50'),
            spaceAfter=10,
            spaceBefore=10,
            fontName='Helvetica-Bold'
        ))
        
        # Überschrift 3 (H3)
        styles.add(ParagraphStyle(
            name='CustomH3',
            parent=styles['Heading3'],
            fontSize=13,
            textColor=colors.HexColor('#34495e'),
            spaceAfter=8,
            spaceBefore=8,
            fontName='Helvetica-Bold'
        ))
        
        # Normal-Text
        styles.add(ParagraphStyle(
            name='CustomBody',
            parent=styles['BodyText'],
            fontSize=11,
            alignment=TA_JUSTIFY,
            spaceAfter=6
        ))
        
        # Code/Inline-Code
        styles.add(ParagraphStyle(
            name='CustomCode',
            parent=styles['BodyText'],
            fontSize=10,
            fontName='Courier',
            textColor=colors.HexColor('#d63384'),
            leftIndent=0.5*cm
        ))
        
        return styles
    
    def _parse_markdown(self, content):
        """Parst Markdown-Content und gibt Platypus-Elemente zurück."""
        elements = []
        lines = content.split('\n')
        i = 0
        
        while i < len(lines):
            line = lines[i]
            
            # Leer-Zeilen überspringen (außer bei Spacer)
            if not line.strip():
                i += 1
                continue
            
            # H1 (# Title)
            if line.startswith('# '):
                text = line[2:].strip()
                elements.append(Paragraph(text, self.styles['CustomH1']))
                elements.append(Spacer(1, 0.3*cm))
                i += 1
            
            # H2 (## Subtitle)
            elif line.startswith('## '):
                text = line[3:].strip()
                elements.append(Paragraph(text, self.styles['CustomH2']))
                elements.append(Spacer(1, 0.2*cm))
                i += 1
            
            # H3 (### Sub-subtitle)
            elif line.startswith('### '):
                text = line[4:].strip()
                elements.append(Paragraph(text, self.styles['CustomH3']))
                elements.append(Spacer(1, 0.15*cm))
                i += 1
            
            # Bullet-Listen (- Item)
            elif line.startswith('- '):
                list_items = []
                while i < len(lines) and lines[i].strip().startswith('- '):
                    item_text = lines[i][2:].strip()
                    # Verschachtelte Punkte (mit Einrückung)
                    indent_level = 0
                    j = i + 1
                    sub_items = []
                    while j < len(lines) and lines[j].startswith('  - '):
                        sub_text = lines[j].strip()[2:].strip()
                        sub_items.append(Paragraph(f"• {sub_text}", self.styles['CustomBody']))
                        j += 1
                    
                    list_items.append(Paragraph(f"• {item_text}", self.styles['CustomBody']))
                    list_items.extend(sub_items)
                    i = max(i + 1, j)
                
                for item in list_items:
                    elements.append(item)
                elements.append(Spacer(1, 0.15*cm))
                continue
            
            # Absätze
            else:
                # Mehrzeilige Absätze sammeln
                paragraph_lines = []
                while i < len(lines) and lines[i].strip() and \
                      not lines[i].startswith('#') and \
                      not lines[i].startswith('- '):
                    paragraph_lines.append(lines[i].strip())
                    i += 1
                
                if paragraph_lines:
                    text = ' '.join(paragraph_lines)
                    # Inline-Code (`text`) formatieren
                    text = self._format_inline_code(text)
                    elements.append(Paragraph(text, self.styles['CustomBody']))
                    elements.append(Spacer(1, 0.15*cm))
                continue
        
        return elements
    
    def _format_inline_code(self, text):
        """Formatiert Inline-Code mit Backticks."""
        # Einfache Ersetzung: `text` → <font face="Courier" color="#d63384">`text`</font>
        import re
        pattern = r'`([^`]+)`'
        replacement = r'<font face="Courier" color="#d63384">`\1`</font>'
        return re.sub(pattern, replacement, text)
    
    def convert(self):
        """Konvertiert Markdown zu PDF."""
        # Markdown-Datei lesen
        with open(self.md_file, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # PDF-Dokument erstellen
        doc = SimpleDocTemplate(
            str(self.pdf_file),
            pagesize=A4,
            rightMargin=2*cm,
            leftMargin=2*cm,
            topMargin=2*cm,
            bottomMargin=2*cm,
            title=self.md_file.stem
        )
        
        # Markdown parsen
        elements = self._parse_markdown(content)
        
        # PDF bauen
        doc.build(elements)
        print(f"✓ PDF erfolgreich erstellt: {self.pdf_file}")


def main():
    parser = argparse.ArgumentParser(
        description='Konvertiert Markdown-Dateien in PDF-Dateien',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog='Beispiel: python md-to-pdf.py H1-561874.md'
    )
    
    parser.add_argument(
        'markdown_file',
        help='Pfad zur Markdown-Datei (.md)'
    )
    
    args = parser.parse_args()
    
    try:
        converter = MarkdownToPDF(args.markdown_file)
        converter.convert()
    except FileNotFoundError as e:
        print(f"Fehler: {e}", file=sys.stderr)
        sys.exit(1)
    except ValueError as e:
        print(f"Fehler: {e}", file=sys.stderr)
        sys.exit(1)
    except Exception as e:
        print(f"Fehler bei der Konvertierung: {e}", file=sys.stderr)
        sys.exit(1)


if __name__ == '__main__':
    main()
