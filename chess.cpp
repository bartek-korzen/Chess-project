#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

    // Struktura reprezentująca szachownicę.
    struct ChessBoard
    {
        enum class Turn
        {
            white, 
            black  
        } turn;
        
        enum class Piece
        {
            king,        
            queen,       
            white_pawn,  
            black_pawn,  
            rook,        
            bishop,      
            knight       
        };
        
        // Statyczny kontener mapujący rodzaj pionka na jego wartość.
        static map<Piece, int> pieceValues;
        
        bool show_coordinates = false; // Zmienna określająca, czy mają być wyświetlane współrzędne na szachownicy.
        
        // Struktura reprezentująca pozycję na szachownicy.
        struct Pos
        {
            int x, y; 
            Pos(const Pos &p, int dx = 0, int dy = 0) // Konstruktor z przesunięciem o dx i dy.
            {
                *this = p;
                x += dx;
                y += dy;
            }
            Pos(int _x, int _y) // Konstruktor ustawiający konkretne współrzędne.
            {
                x = _x;
                y = _y;
            }
            // Operator porównujący potrzebny do używania klasy 'Pos' w kontenerach, sortowaniu itp.
            bool operator<(const Pos &p) const { return (x < p.x) || (x == p.x && y < p.y); }
            // Operator sprawdzający równość pozycji.
            bool operator==(const Pos &p) const { return x == p.x && y == p.y; }
            // Konstruktor domyślny, inicjujący współrzędne na wartości -1.
            Pos()
            {
                x = -1;
                y = -1;
            }
        };

        map<Pos, Piece> white_pieces, black_pieces; // Mapy przechowujące pozycje białych i czarnych pionków.
        
        // Funkcja zwracająca referencję do mapy pionków aktualnego gracza wykonującego ruch.
        map<Pos, Piece> &moverPieces() { return turn == Turn::white ? white_pieces : black_pieces; }
        
        // Funkcja zwracająca referencję do mapy pionków przeciwnika aktualnego gracza.
        map<Pos, Piece> &opponentPieces() { return turn == Turn::white ? black_pieces : white_pieces; }

     // Funkcja inicjuje planszę szachową, ustawiając początkowe pozycje pionków dla obu graczy oraz usuwając ewentualne wcześniejsze pozycje.
    void reset()
    {
        turn = Turn::white; // Ustawienie tury na białe.
        white_pieces.clear(); 
        black_pieces.clear(); 
        // Ustawienie początkowych pozycji pionków pionków dla obu graczy.
        for (int i = 1; i < 9; ++i)
        {
            white_pieces[Pos(i, 7)] = Piece::white_pawn; // Pionki białe na dolnym rzędzie.
            black_pieces[Pos(i, 2)] = Piece::black_pawn; // Pionki czarne na górnym rzędzie.
        }
        int n = 1;
        // Ustawienie pozycji pozostałych pionków dla obu graczy.
        for (auto piece : {Piece::rook, Piece::knight, Piece::bishop, Piece::king})
        {
            white_pieces[Pos(n, 8)] = white_pieces[Pos(9 - n, 8)] = black_pieces[Pos(n, 1)] = black_pieces[Pos(9 - n, 1)] = piece;
            ++n;
        }
        white_pieces[Pos(4, 8)] = black_pieces[Pos(4, 1)] = Piece::queen; 
    }

    // Funkcja zmienia turę gry, przechodząc od białych do czarnych lub od czarnych do białych.
    void flipTurn() { turn = turn == Turn::white ? Turn::black : Turn::white; }

    // Funkcja sprawdza, czy ruch z pozycji from na pozycję to jest możliwy, usuwa pionka przeciwnika, przenosi pionka gracza wykonującego ruch, a następnie zmienia turę na przeciwną.
    bool makeMove(Pos from, Pos to)
    {
        vector<Pos> allowed = possibleMoves(from); // Uzyskanie możliwych ruchów dla danej pozycji.
        if (find(allowed.begin(), allowed.end(), to) == allowed.end()) // Sprawdzenie, czy ruch jest dozwolony.
            return false;
        opponentPieces().erase(to); // Usunięcie pionka przeciwnika, jeśli został zbity.
        moverPieces()[to] = moverPieces()[from]; // Przeniesienie pionka na nową pozycję.
        moverPieces().erase(from); // Usunięcie pionka z poprzedniej pozycji.
        if ((moverPieces()[to] == Piece::white_pawn || moverPieces()[to] == Piece::black_pawn) && (to.y == 1 || to.y == 8))
            moverPieces()[to] = Piece::queen; 
        flipTurn(); // Zmiana tury na przeciwną.
        return true; // Zwrócenie informacji o poprawnym wykonaniu ruchu.
    }

    // Funkcja zwracająca wektor możliwych ruchów dla danego pionka.
        vector<Pos> possibleMoves(const Pos &from)
        {
            vector<Pos> moves; // Wektor przechowujący możliwe ruchy.
            
            // Funkcje lambda sprawdzające różne warunki dotyczące ruchu.
            auto isOwn = [&](int dx, int dy) -> bool
            { return moverPieces().count(Pos(from, dx, dy)); }; // Sprawdza, czy na danej pozycji znajduje się pionek gracza.
            
            auto isOpponent = [&](int dx, int dy) -> bool
            { return opponentPieces().count(Pos(from, dx, dy)); }; // Sprawdza, czy na danej pozycji znajduje się pionek przeciwnika.
            
            auto isInsideBoard = [&](int dx, int dy) -> bool
            { Pos p(from,dx,dy); return p.x < 9 && p.x > 0 && p.y < 9 && p.y > 0; }; // Sprawdza, czy pozycja znajduje się na planszy.
            
            auto isFree = [&](int dx, int dy) -> bool
            { return !isOwn(dx, dy) && isInsideBoard(dx, dy) && !isOpponent(dx, dy); }; // Sprawdza, czy pole jest wolne.
            
            // Dodaje możliwy ruch do wektora ruchów.
            auto addMove = [&](int dx, int dy) -> bool
            {
                if (isFree(dx, dy) || isOpponent(dx, dy)) // Jeśli pole jest wolne lub zajęte przez przeciwnika.
                {
                    moves.push_back(Pos(from, dx, dy)); // Dodaj ruch do wektora.
                    return true; // Zwróć true, że ruch został dodany.
                }
                return false; // Zwróć false, że ruch nie został dodany.
            };

            if (!isOwn(0, 0)) // Jeśli na danej pozycji nie znajduje się pionek gracza.
                return moves; // Zwróć pustą listę ruchów.

            auto moving_piece = moverPieces()[from]; // Pobierz typ pionka, który się rusza.

        // Sprawdzanie możliwych ruchów dla różnych rodzajów pionków.    
                    switch (moving_piece)
            {
                // Dla białego pionka.
                case Piece::white_pawn:
                    if (isFree(0, -1))
                        addMove(0, -1); // Ruch do przodu o jedno pole.
                    if (isFree(0, -1) && isFree(0, -2) && from.y == 7)
                        addMove(0, -2); // Ruch do przodu o dwa pola, jeśli pion znajduje się na swoim początkowym polu.
                    if (isOpponent(-1, -1))
                        addMove(-1, -1); // Atak na skos w lewo.
                    if (isOpponent(1, -1))
                        addMove(1, -1); // Atak na skos w prawo.
                    break;

                // Dla czarnego pionka.
                case Piece::black_pawn:
                    if (isFree(0, 1))
                        addMove(0, 1); // Ruch do przodu o jedno pole.
                    if (isFree(0, 1) && isFree(0, 2) && from.y == 2)
                        addMove(0, 2); // Ruch do przodu o dwa pola, jeśli pion znajduje się na swoim początkowym polu.
                    if (isOpponent(-1, 1))
                        addMove(-1, 1); // Atak na skos w lewo.
                    if (isOpponent(1, 1))
                        addMove(1, 1); // Atak na skos w prawo.
                    break;

                // Dla skoczka.
                case Piece::knight:
                    // Wszystkie możliwe ruchy dla skoczka.
                    addMove(-2, -1);
                    addMove(-2, 1);
                    addMove(2, -1);
                    addMove(2, 1);
                    addMove(-1, -2);
                    addMove(-1, 2);
                    addMove(1, -2);
                    addMove(1, 2);
                    break;

                // Dla króla.
                case Piece::king:
                    // Wszystkie możliwe ruchy dla króla.
                    for (auto dy : {-1, 0, 1})
                        for (auto dx : {-1, 0, 1})
                            addMove(dy, dx);
                    break;

                // Dla hetmana i wieży.
                case Piece::queen:
                case Piece::rook:
                    // Ruchy w pionie i poziomie.
                    for (int n = 1; n < 9 && addMove(0, n) && !isOpponent(0, n); ++n)
                        ;
                    for (int n = 1; n < 9 && addMove(0, -n) && !isOpponent(0, -n); ++n)
                        ;
                    for (int n = 1; n < 9 && addMove(n, 0) && !isOpponent(n, 0); ++n)
                        ;
                    for (int n = 1; n < 9 && addMove(-n, 0) && !isOpponent(-n, 0); ++n)
                        ;
                    // Jeśli pionek to hetman, to wykonaj również ruchy na skos.
                    if (moving_piece != Piece::queen)
                        break;

                // Dla gońca i hetmana.
                case Piece::bishop:
                    // Ruchy na skos.
                    for (int n = 1; n < 9 && addMove(n, n) && !isOpponent(n, n); ++n)
                        ;
                    for (int n = 1; n < 9 && addMove(n, -n) && !isOpponent(n, -n); ++n)
                        ;
                    for (int n = 1; n < 9 && addMove(-n, n) && !isOpponent(-n, n); ++n)
                        ;
                    for (int n = 1; n < 9 && addMove(-n, -n) && !isOpponent(-n, -n); ++n)
                        ;
                    break;
            }

        return moves;
    }
    
    // Funkcja wyświetlająca aktualny stan szachownicy w konsoli.
    void printBoard()
    {
        static map<Piece, char> sprites =
            {{Piece::white_pawn, 'P'}, {Piece::black_pawn, 'P'}, {Piece::rook, 'H'}, {Piece::knight, 'F'}, {Piece::bishop, 'I'}, {Piece::king, 'K'}, {Piece::queen, 'Q'}};
        
        cout << endl
            << "                        Strona Czarna" << endl
            << endl
            << "        1     2     3     4     5     6     7     8   " << endl;
        
        cout << "      _____ _____ _____ _____ _____ _____ _____ _____ ";
        
        for (int y = 1; y < 9; ++y)
        {
            if (show_coordinates)
                cout << endl
                    << "     |1" << y << "   |2" << y << "   |3" << y << "   |4" << y << "   |5" << y << "   |6" << y << "   |7" << y << "   |8" << y << "   |";
            else
                cout << endl
                    << "     |     |     |     |     |     |     |     |     |";
            
            cout << endl
                << "  " << y << "  ";
            for (int x = 1; x < 9; ++x)
            {
                cout << "|  ";
                // Sprawdzenie czy na danym polu znajduje się pionek biały.
                if (white_pieces.count(Pos(x, y)))
                    cout << sprites[white_pieces[Pos(x, y)]];
                // Sprawdzenie czy na danym polu znajduje się pionek czarny.
                else if (black_pieces.count(Pos(x, y)))
                    cout << (char)tolower(sprites[black_pieces[Pos(x, y)]]);
                else
                    cout << " "; 
                cout << "  ";
            }
            cout << "|  " << y << endl
                << "     |_____|_____|_____|_____|_____|_____|_____|_____|";
        }
        cout << endl
            << "        1     2     3     4     5     6     7     8   " << endl
            << endl
            << "                       Strona Biała" << endl
            << endl;
    }

    void printHelp()
    {
        cout << endl
             << "Aby wyświelić centrum pomocy wciśnij 'h', aby wyjść z gry wciśnij 'q', aby zobaczyć planszę wciśnij 'p',"<< endl
             << "aby zobaczyć koordynaty poszczególnych pól na planszy wciśnij 'c'" << endl
             << endl
             << "Aby poruszać się pionkiem, w konsoli należy wpisać wartości koordynatów w sposób 'yxyx' na przykład: '1715' przemieszcza figurę z pola (x,y)=(1,7) na (x,y)=(1,5)" << endl
             << endl;
    }

    // Funkcja do pobrania ruchu od użytkownika.
    bool promptInput()
    {
        string move; // Zmienna przechowująca ruch użytkownika.
    illegalmove:
        // Wyświetlenie komunikatu o kolejności gracza.
        if (turn == Turn::white)
            cout << "Kolej białych: ";
        else
            cout << "Kolej czarnych: ";
        
        // Pobranie ruchu od użytkownika.
        if (move == "")
            cin >> move;
        
        // Obsługa różnych poleceń użytkownika.
        if (move == "q") 
        {
            cout << "Do zobaczenia ponownie!" << endl
                 << endl;
            return false;
        }
        if (move == "?" || move == "h" || move == "help") 
        {
            printHelp(); 
            move = "";
            goto illegalmove; 
        }
        if (move == "c") 
        {
            show_coordinates = !show_coordinates; 
            printBoard(); 
            move = "";
            goto illegalmove; 
        }

        if (move == "p") 
        {
            printBoard(); 
            move = "";
            goto illegalmove; 
        }
        
        // Wczytanie współrzędnych ruchu od użytkownika.
        Pos from(-1, -1), to(-1, -1);
        if (move.length() == 4)
        {
            from.x = move[0] - '0';
            from.y = move[1] - '0';
            to.x = move[2] - '0';
            to.y = move[3] - '0';
        }
        
        // Sprawdzenie czy ruch jest dozwolony, jeśli nie, wyświetlenie komunikatu.
        if (!makeMove(from, to))
        {
            cout << "! Niedozwolony ruch !" << endl;
            move = "";
            goto illegalmove; // Ponowne pobranie ruchu.
        }
        
        printBoard(); // Wyświetlenie planszy po wykonaniu ruchu.
        return true; // Zwrócenie wartości true, oznaczającej, że użytkownik nadal gra.
    }

    // Funkcja obliczająca punktację na szachownicy.
    int score()
    {
        int sumWhite = 0;
        for (auto &p : white_pieces)
            sumWhite += pieceValues[p.second];
        int sumBlack = 0;
        for (auto &p : black_pieces)
            sumBlack += pieceValues[p.second];
        return sumWhite - sumBlack; // Zwrócenie różnicy punktacji białych i czarnych pionków.
    }

    // Funkcja sprawdzająca, czy na planszy znajduje się król.
    bool hasKing()
    {
        for (auto &p : moverPieces())
            if (p.second == Piece::king)
                return true;
        return false; // Jeśli nie ma króla na planszy.
    }

    // Struktura reprezentująca ruch.
    struct Move
    {
        Pos from, to; // Pozycje początkowa i końcowa ruchu.
        int score; // Punkty ruchu.
    };

    // Algorytm minimax do szukania najlepszego ruchu.
    Move minimax(int depth, bool minimize)
    {
        Move best_move; // Najlepszy ruch.
        best_move.score = -1000000 + 2000000 * minimize; // Inicjalizacja najlepszej punktacji.

        // Jeśli osiągnięto maksymalną głębokość przeszukiwania, oblicz punktację aktualnej pozycji.
        if (0 == depth)
        {
            best_move.score = score(); // Oblicz punktację aktualnej pozycji.
            return best_move; // Zwróć najlepszy ruch.
        }

        // Przeszukiwanie wszystkich możliwych ruchów.
        for (auto &from : moverPieces())
        {
            for (auto &to : possibleMoves(from.first))
            {
                ChessBoard branch = *this; // Utworzenie kopii szachownicy.
                branch.makeMove(from.first, to); // Wykonanie ruchu.
                Move option = branch.minimax(depth - 1, !minimize); // Rekurencyjne wywołanie minimax dla kolejnego poziomu.
                
                // Aktualizacja najlepszego ruchu na podstawie wyników kolejnych poziomów.
                if ((option.score > best_move.score && !minimize) || (option.score < best_move.score && minimize))
                {
                    best_move.score = option.score;
                    best_move.from = from.first;
                    best_move.to = to;
                }
            }
        }
        return best_move; // Zwróć najlepszy ruch.
    }

    // Funkcja wykonująca ruch komputera.
    void AIMove()
    {
        bool minimize = turn == Turn::black ? true : false; // Określenie czy minimalizujemy czy maksymalizujemy wynik.
        Move m = minimax(1, minimize); // Wywołanie algorytmu minimax.
        makeMove(m.from, m.to); // Wykonanie najlepszego ruchu.
        printBoard(); // Wyświetlenie planszy po wykonaniu ruchu.
    }
};

map<ChessBoard::Piece, int> ChessBoard::pieceValues{
    // Mapa wartości poszczególnych figur szachowych.
    {ChessBoard::Piece::king, 10000},     
    {ChessBoard::Piece::queen, 9},        
    {ChessBoard::Piece::black_pawn, 1},   
    {ChessBoard::Piece::white_pawn, 1},   
    {ChessBoard::Piece::bishop, 3},       
    {ChessBoard::Piece::knight, 3},       
    {ChessBoard::Piece::rook, 5},         
};

int main()
{
    ChessBoard game; // Inicjalizacja obiektu szachownicy.
    cout << endl
         << "Gra w Szachy" << endl; // Wyświetlenie tytułu gry.
    game.printHelp(); 
    bool gameon = true; // Flaga oznaczająca trwanie gry.
    while (gameon) 
    {
        game.reset(); // Zresetowanie stanu szachownicy.
        string pick_side = "";
        while (pick_side != "b" && pick_side != "w" && pick_side != "q")
        {
            cout << endl
                 << "Wybierz stronę: Czarna (wpisz 'b'), Biała (wpisz 'w'), jeśli chcesz wyjść z gry wciśnij 'q': ";
            cin >> pick_side;
        }
        if (pick_side == "q") 
        {
            cout << "Do zobaczenia następnym razem!" << endl; 
            break; 
        }

        if (pick_side == "b") 
            game.AIMove(); 
        else
            game.printBoard(); 

        while (gameon = game.promptInput()) 
        {
            if (!game.hasKing()) 
            {
                cout << "* Gratulacje, wygrałeś!" << endl; 
                break; 
            }
            game.AIMove(); 
            if (!game.hasKing()) 
            {
                cout << "* Przegrałeś, powodzenia następnym razem!" << endl; 
                break; 
            }
        }
    }
    return 0; 
}
