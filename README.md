# Schachbot mit hybrider Board Darstellung

## [Video](todo)

## grundlegende Projektfunktionalität

### grundlegender Aufbau

* Der Schachbot nutzt den bekannten MinMax Algorithmus mit Alpha Beta Pruning 
 und einer Evaluation, die Material, PST (Piece-Square Table), und King Safety berücksichtigt.
* Das Board besteht aus einer Hybriden Darstellung, die eine "Piece Centric"-Darstellung (Piece Lists)
 und eine "Square Centric"-Darstellung (8x8 Board mit einem 64 element großem Array) kombiniert

### grundlegende Funktionalität im Terminal


#### Move Making
* `[Move]` mit der gleichen Move-Syntax wie in den Exercises
  * Syntax: `[Move] := [PieceToMove][SourceSquare]([capture]?)[targetSquare]([promotion]?)`
  * Beispiele: `Rh1f1` (normal move), `Pe4xf5` (capture), `Pa7a8=Q` (promotion), `Ke1c1` (castling)


#### commands
Während der Ausführung des Programms im Terminal stehen auch noch folgende Befehle zur Verfügung:

* `/fromFen [FEN]` setzt das Schachbrett auf eine Stellung gemäß der angegebenen Forsyth-Edwards-Notation (FEN).
  * mit `[FEN]` als ein Schachbrett in Forsyth-Edwards Notation (FEN).
  * z.B. `/fromFen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1`
* `/toFen` gibt das jetzige Schachbrett in Forsyth-Edwards Notation aus.
* `/allMoves` listet alle zur Zeit möglichen Moves in einem Raster mit 5 Spalten auf.
  * alle ausgegebenen Moves sind in der gleichen Move-Syntax wie in [Move Making](#Move-Making) 
  und aktuell legale Moves, d.h. Moves die den Spieler nicht in CheckMate setzen
* `/bestMove` gibt mithilfe des eingebauten MinMax Algorithmus den derzeit besten Move zurück
* `/evaluate` gibt die Evaluation (die auch in MinMax benutzt wird) für das aktuelle Schachbrett zurück
  * Positive Werte zeigen einen Vorteil für Weiß, negative Werte einen Vorteil für Schwarz
* `/undo` setzt, falls möglich, die letzten zwei Moves zurück, d.h. der letzte Bot und der letzte Spieler Move werden rückgängig gemacht.


## wichtigste Klassen und Funktionen

### Board


| Wo?    | `src/Board/Board.h`, und allen Dateien in `src/Board/Board_impl/`, (+ `src/Board/Board_Correctness.h` zum Testen)                                                                                                                                                                 |
|--------|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| Was?   | Das Schachbrett sozusagen auf dem man unter anderem einen Move ausführen kann mit  `movePiece()` und einen Move zurücksetzen kann mit `undoMove()`, und Zugriff bekommt auf dessen unterliegenden Schachbrett Darstellungen, welche wiederum weitere hilfreiche Methoden besitzen |
| wofür? | Kapselung der Teilweise komplizierten Logik hinter einem Move in relativ einfache, verständliche public Methoden, um über diese dann viele Funktionalitäten in Schach zu realisieren                                                                                              |



### FenParsing mit der Fen und FenStringReader Klasse


| Wo?    | `src/IO/Perser/FenParsing.h`, `src/IO/Perser/FenParsing.cpp`  und `src/IO/Perser/FenParsingException.h`                                                                                                               |
|--------|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| Was?   | Funktionalität um einen Fen einzulesen (`Fen::buildFenFromStr()`), wobei jeder eigene Teil des FEN in einer eigenen kleinen Funktion geparst und getestet wird mittels der dabei hilfreichen `FenStringReader` Klasse |
| wofür? | Um die Benutzereingabe abgesichert durchgehen zu können, um auf nicht legale FEN zu prüfen, und um auch Fen zu akzeptieren, die auch etwas komisch aussehen können, aber trotzdem ein eindeutiges Schachbrett liefern |


### King Check Funktionalität

| Wo?    | `src/Check/Check.h` und `src/Check/Check.cpp`                                                                                                           |
|--------|---------------------------------------------------------------------------------------------------------------------------------------------------------|
| Was?   | einfache Funktion, die einfach nur berechnet ob ein gegebener König gerade im Check steht oder nicht `Check::isChecked()`                               |
| wofür? | wird zum Testen von CheckMate und zur erzeugen von legalen Moves die den eigenen King nicht in Schach Stellen benutzt, sowie innerhalb MinMax und Perft |



### PossibleMoves Namespace

| Wo?    | `src/PossibleMoves/PossibleMoves.h` und `src/PossibleMoves/PossibleMoves.cpp`                                                                                                                                                                                                                           |
|--------|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| Was?   | Funktionalität um alle Pseudo legalen Moves für ein bestimmtes Schachbrett und einem bestimmten Team zu erzeugen (`PossibleMoves::getAllPossibleMoves()`). Und Funktionalität im davon Moves, die sich selbst in CheckMate setzen zu entfernen (`PossibleMoves::trimMovesPuttingPlayerIntoCheckmate()`) |
| wofür? | Für das erzeugen von möglichen Moves für MinMax und Perft und dem Testen, dass der Spieler auch wirklich legale Moves macht                                                                                                                                                                             |



### Bot

| Wo?    | `src/Bot/MinMax/` und `src/Bot/Evaluation/`                                                                                                                                                                                                                                                                      |
|--------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| Was?   | Die Funktionalität für einen Schachbot. Einmal eine Implementation eines MinMax Algorithmus, der nicht `undoMove()` benutzt sondern immer das Board kopiert weil undo ca. 10% langsamer war in Perft. Und Der Am Anfang schon genannten Evaluationsfunktion mit der man verschiedene Boards im MinMax vergleicht |
| wofür? | Zur Implementierung des Projektziels, einem Schach Bot im Terminal                                                                                                                                                                                                                                               |



### ChessGame


| Wo?    | `src/ChessGame.h`                                                                                    |
|--------|------------------------------------------------------------------------------------------------------|
| Was?   | eine Art Wrapper Klasse um alle andere Funktionalität, für eine einfache und einheitliche public API |



### MainGameLoop


| Wo?  | `src/MainGameLoop.h` und  `src/MainGameLoop.cpp`                                                                                                                                               |
|------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| Was? | Der Loop des Schach Spiels im Terminal mit den am Anfang vorgestellten Commands, weitere Logik zum Schachbrett einlesen, und das richtige Handhaben bei einem Sieg oder Verlieren des Spielers |


## Programmierkonzepte

### Lambdas
* Vereinfacht das Wiedernutzen von bestimmten Operationen innerhalb einer Funktion an vielen Stellen ohne sie sie in eine eigene Funktion aufgeteilt werden muss
* Ermöglicht das Templitisieren von Funktionsteilen zu eigenen Funktionen, die als Argument einfach eine Lambda erhalten und diese an vorbestimmten Stellen aufrufen
* Verwendung in folgender Datei: `PossibleMoves.cpp`

### Templates
* Minimieren Code wiederholung, durch Zusammenfassung in eine eigene template Funktion mit template argumenten wie z.B. einem Lambda als argument
* lässt gleiche Funktionalität für verschiedene unterliegende Typen in einer Klasse bündeln, die man spezialisieren kann, wodurch das Nutzen dieser spezialisierten Klassen sich immer gleich anfühlt, auch wenn die Implementation Unterschiede hat
* Verwendung in den folgenden Dateien: `PossibleMoves.cpp`, `Evaluate.cpp`, und `SlimOptional.h`

### Exceptions
* Vereinfachen den Kontrollfluss beim Verarbeiten von Benutzereingaben
* Verwendung in den folgenden Dateien: `FenParsingExceptions.h`, `FenParsing.cpp`,  und `FenParsing.h`

### namespaces
* Verhindern Namenskonflikte, zeigen oftmals ursprünge von verschiedenen Funktionen, und verbessern Lesbarkeit von Code
* Bündeln ähnliche Funktionalität, undUmgehen das mögliche Problem einer Instantiierung einer komplett statischen Klasse
* Verwendung in den folgenden Dateien: `Print.h`, `PossibleMoves.h`, `Check.h`, `CheckMate.h`, und `BasicChessTypes.h`

### Objektorientierung
* Durch die Kapselung von "Implementation Details" in Klassen wird der Zugriff auf Variablen und Aufruf von Funktionen verhindert, die nicht außerhalb der Klasse benötigt werden oder sogar außerhalb der Klasse nicht aufgerufen werden sollen
* An Klassen lassen sich einfach Funktionen namens Methoden hängen, die das Nutzen von Funktionalität vereinfachen
* Ich hab mich aktiv in meinem Programmdesign gegen das Nutzen von Dynamic Dispatch entschieden, da es hier nur Indirections hinzufügen, und die Größe von Objekten vergrößern würde, ohne viele Vorteile zu bringen.
* Verwendung in den folgenden Dateien: `Board.h`, `Board_8x8.h`, `Board_Extra.h`, `Board_Positions.h`, `FenParsing.h`, `ChessGame.h`, und in den meisten structs in `/Types`, da structs in C++ von ihrer Funktionalität fast dasselbe sind wie Klassen.

### Operator Overloading
* vereinfacht oftmals das Nutzen und Verstehen der Funktionalität einiger Klassen
* Ermöglicht Checks einzubauen für manche gefährliche, aber oft benutzen Operatoren, ohne dass der Nutzer davon wissen muss (i.e. Bounds Checks bei "\[\]")
* Verwendung in den folgenden Dateien: `FenParsing.h`, `Board_8x8.h`, `Vec2.h`, und `BasicChessTypes.h`

### Asserts
* Ermöglicht es Erwartungen an bestimmten Stellen im Code zu "asserten", wodurch oft unerwartete Bugs oder falsche Code-Logik relativ schnell gefunden werden können
* Verwendung in fast jeder Datei in diesem Projekt aber vor allem: `Board.h`, und `Board_Positions.h`

## Libraries

### gtest
In diesem Projekt wird [gtest](https://github.com/google/googletest) (Goggle's Testing Library) zum automatisierten Testen von Funktionalität verwendet

#### Wo?
gtest wird im `Test/gtest/` verwendet, um die Korrektheit des Programms während des Code-Schreibens periodisch zu testen und mögliche Bugs frühzeitig zu fixen

#### Warum?
* gtest is eine bekannte, und schon lang existierende Test Library für C++, wodurch es sehr gute Docs hat und man bei vielen Problemen schnell eine Lösung finden kann.
* Des Weiterem ist das Schreiben von Tests in gtest sehr anfängerfreundlich und einfach, wodurch man relativ schnell hilfreiche Tests implementiert bekommt

## eingebaute Tests

> [!IMPORTANT]  
> Damit die gtest Tests funktionieren muss man den Pfad (variable: `main_folder_path`) in `project/Test/Main_Folder_Path_For_Testing.h` anpassen

### Asserts
In diesem Projekt werden Asserts unter anderem auf 4 große Weisen benutzt

#### 1: passende Funktionsargumente
Um sicherzustellen, dass Funktionen mit korrekten Argumenten aufgerufen werden.
Dies ist besonders wichtig bei Funktionen, die bestimmte "prerequisites" für ihre Argumente haben damit sie korrekt arbeiten können.
z.B. bei `Team::getEnemyTeam(Team team)` wo das Argument `Team::NONE` nicht erlaubt ist, da None kein Gegnerteam hat

#### 2: out of bounds checks
Um sicherzustellen, dass nicht ausserhalb eines Arrays geschrieben oder gelesen wird während des normalen Programmablaufes.
Dadurch verhindert man schwer zu findende Bugs, "undefined behaviour", mögliche Programmabstürze, oder sogar Sicherheitsprobleme

#### 3: Gleichheit zwischen beiden Board Darstellungen
Um Sicherzustellen, dass beide Darstellungen des Schachbretts, also die "Piece Centric"-Darstellung und "Piece Centric"-Darstellung, sich nicht unterscheiden in den Pieces und deren Position die sie halten.
Dies wird vor und nach jedem Move und undoMove geprüft, wenn Asserts aktiviert sind.

### gtest

#### einfache Tests für BasicChessTypes
Ein Paar einfache Tests die ich anfänglich geschrieben habe, um die Korrektheit der Funktionalität innerhalb dieser Datei zu prüfen

#### Perft ohne Undo
Test: generiert und macht alle möglichen legalen Moves bis zu einer bestimmten Tiefe und zählt die Anzahl der Knoten auf dieser Endtiefe.
Diese Anzahl kann man dann mit, schon bekannten, und verifizierten Werten vergleichen, um die Korrektheit eines Chessbots zu testen.

Hiermit testet man:
* die Korrekte Move Generierung von Pseudo Legal Moves in `PossibleMoves.cpp`
* die King Check Funktionalität in `Check.cpp`
* die Korrektheit der Ausführung von Moves auf dem Board
* begrenzt: das einlesen und verarbeiten von FEN Inputs, und korrekte Spieler Züge durch Wiederbenutzen von `PossibleMoves`.
* indirekt: `Vec2.h`, `SlimOptional.h`, und alle Funktionalität `BasicChessTypes.h` außer `Piece::asChar()`, da diese sehr viel im Perft Test zum Berechnen der Boards und Moves benutzt werden.

#### Perft mit Undo
Test: derselbe grundlegende Test wie Perft ohne Undo, doch hier benutzt man die undo Funktionalität des Boards, anstelle von Kopieren des Boards bei jedem neuen Funktionsaufruf

Hiermit testet man:
 * alle Funktionalität die durch Perft ohne undo getestet wurde
 * die Korrektheit der UndoMove Funktionalität in der `Board` Klasse. Wodurch das `Board` und dessen implementierten Klassen `Board_8x8`, `Board_Extra`, und `Board_Positions` komplett getestet sind

#### Fen Parsing und Ausgabe
Test: es werden alle gegebenen FEN in dem Perft Dataset gelesen, geparst, wieder zurück in Strings verwandelt, und verglichen, damit man sozusagen Zeigen kann, 
dass das Aufrufen dieser Funktionalität nacheinander eine Art Identitätsfunktion bildet, und somit höchstwahrscheinlich korrekt ist

Hiermit testet man:
* das korrekte Einlesen von FEN
* die korrekte Ausgabe von FEN
* indirekt: die korrekte Ausgabe von `Piece::asChar()`, da diese während der Ausgabe vom FEN intern benutzt wird

#### Fen und Move Parsing
Es gibt ein Fen, dann einen legalen Move auf dem diesem FEN, und dann gibt es noch einen Zweiten Fen, der das Board nach diesem Move darstellt.
Man liest all diese Ein und testet, wie bei Fen Parsing und Ausgabe, dass die Identitätsfunktion gilt.
Danach führt man den Move auf dem Board aus und testet ob der ausgegebene Fen gleich ist wie der Fen des Schachbretts nach dem Move

Hiermit testet man:
* dasselbe wie bei Fen Parsing und Ausgabe nochmals auf eine größere Anzahl von FEN
* das korrekte Einlesen und Verarbeiten von Benutzer Moves

#### Einfacher Test für die Board-Evaluation
Da die Evaluationsfunktion nur schwer testbar ist, hab ich ein paar Testfälle geschrieben, um grundlegende Logik zu testen.

Dieser Test liest ein Board aus einem FEN ein und schaut welcher Score gerade existiert und vergleicht ihn mit zu erwartenden Werten.

Hiermit testet man:
* Bei offensichtlich nicht existierendem Vorteil für beide Seiten der ausgegebene Score 0 ist.
* dass bei manchen Schachbrettstellungen die offensichtlich einen Vorteil für eine bestimmte Farbe haben, diese Farbe auch einen bemerkbaren Vorteil im Score hat


## Fremdcode und Datasets

#### Eval

* Piece Value, Mid Game Factor und Piece-Square Tables in `pesto_tables.h` stammen aus [PeSTO's Evaluation Function](https://www.chessprogramming.org/PeSTO%27s_Evaluation_Function)
und wurden nur von der Reihenfolge her angepasst, da sich die Piece type Reihenfolge unterscheidet zwischen diesem Projekt und PeSTO's Evaluation Function.
  * Diese Daten werden benutzt, weil sie schon gut für Chess-Bots getestet und optimiert worden sind, und man dadurch einen besseren Bot erwarten kann, als wenn man selber versucht sich Werte auszudenken

* Des Weiterem wurde von [PeSTO's Evaluation Function](https://www.chessprogramming.org/PeSTO%27s_Evaluation_Function) die "tapered eval" in `Eval.cpp` übernommen und angepasst. Grund hierfür ist, dass es eine gut getestete Formel ist, die einen guten Bot erwarten lässt.
  * midgamePhase wird in diesem Projekt gleich auf das Minimum zwischen gamePhase und 24 gesetzt, um die Idee dahinter zu verdeutlichen, 
dass wenn ein oder mehrere Teams ohne viele Pieces verloren zu haben ihre Pawns zu promoten die MidgamePhase über 24 ansteigen kann und somit auch egPhase unter 0 sinken kann, wodurch unerwartete Scores entstehen können
  * der berechnete Score aus der tapered eval wird nicht mehr durch 24 geteilt, da wenn der Score in eine Int passt, wird einfach nur etwas genauigkeit verloren, ohne Irgendwas zu gewinnen, wenn man bei minMax nur Werte vergleicht die aus dieser Eval kommen

#### Perft Test Data
* Auch wurden die Perft Daten für das Testen und Debuggen des Programms von [Ethereal](https://github.com/AndyGrant/Ethereal/tree/master/src/perft) übernommen, damit man die Korrektheit des Programms testen kann
