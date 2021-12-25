#include <iostream>
#include <fstream>
#include <string>

delegate void MessageDelegate(std::string);
delegate void ActionDelegate();

ref class Player {
private:
    const int HP;
    int healthPoints;
public:
    Player() : HP(100) {
        this->healthPoints = 100;
    }

    Player(int healthPoints) : HP(healthPoints) {
        this->healthPoints = healthPoints;
    }

    void restorePlayer() {
        this->healthPoints = HP;
    }

    int getHealthPoints() { return healthPoints; }

    void hit(Player^ player) {
        player->healthPoints -= rand() % 20;
    }
};

ref class Statistic {
private:
    int countWinPlayerOne = 0;
    int countWinPlayerTwo = 0;
public:
    void winPlayerOne() { countWinPlayerOne++; }
    void winPlayerTwo() { countWinPlayerTwo++; }
    int getCountWinPlayerOne() { return countWinPlayerOne; }
    int getCountWinPlayerTwo() { return countWinPlayerTwo; }
};

ref class Logger {
public:
    void log(std::string str) {
        std::ofstream file;
        file.open("fileName.txt", std::ios::app);
        file << str;
        file.close();
    }
};

ref class Room {
private:
    int idRoom = 0;
    Player^ playerOne;
    Player^ playerTwo;

    std::string getTime(void) {
        time_t now = time(0);
        tm timeinfo;
        localtime_s(&timeinfo, &now);
        char buffer[9];
        strftime(buffer, 9, "%H:%M:%S", &timeinfo);
        std::string str_buffer(buffer);
        return str_buffer;
    }
public:
    event ActionDelegate^ WinPlayerOneEvent;
    event ActionDelegate^ WinPlayerTwoEvent;
    event MessageDelegate^ WinEvent;
    event MessageDelegate^ AddPlayerEvent;
    event MessageDelegate^ DeletePlayerEvent;
    event MessageDelegate^ StartGameEvent;
    event MessageDelegate^ EndGameEvent;

    Room(int idRoom) { this->idRoom = idRoom; }
    Room(Player^ playerOne, Player^ playerTwo, int idRoom) {
        this->playerOne = playerOne;
        this->playerTwo = playerTwo;
        this->idRoom = idRoom;

    }
    Room(int healthPoints, int idRoom) {
        this->playerOne = gcnew Player(healthPoints);
        this->playerTwo = gcnew Player(healthPoints);
        this->idRoom = idRoom;
    }
    void addPlayerOne(Player^ player) {
        this->playerOne = player;
        std::string message = "�������� ����� �1 � ������� ";
        message += std::to_string(idRoom) + "\n";
        AddPlayerEvent(message);
    }
    void addPlayerTwo(Player^ player) {
        this->playerTwo = player;
        std::string message = "�������� ����� �2 � ������� ";
        message += std::to_string(idRoom) + "\n";
        AddPlayerEvent(message);
    }

    void deletePlayerOne() {
        delete this->playerOne;
        std::string message = "����� ����� �1 �� ������� ";
        message += std::to_string(idRoom) + "\n";
        DeletePlayerEvent(message);
    }
    void deletePlayerTwo() {
        delete this->playerTwo;
        std::string message = "����� ����� �2 �� ������� ";
        message += std::to_string(idRoom) + "\n";
        DeletePlayerEvent(message);
    }

    void restorePlayerS() {
        this->playerOne->restorePlayer();
        this->playerTwo->restorePlayer();
    }

    Player^ getPlayerOne() { return playerOne; }
    Player^ getPlayerTwo() { return playerTwo; }

    Player^ Play() {
        if (playerOne && playerTwo) {
            restorePlayerS();
            std::string str = "***������ ���� -> " + getTime() + "\n";
            StartGameEvent(str);

            Player^ first = playerOne;
            Player^ second = playerTwo;

            int lot = rand() % 2;
            if (lot != 0) {
                first = playerTwo;
                second = playerOne;
            }

            bool flag = true;
            while (first->getHealthPoints() > 0 && second->getHealthPoints() > 0) {
                if (flag) {
                    first->hit(second);
                }
                else {
                    second->hit(first);
                }
                flag = !flag;
            }
            if (playerOne->getHealthPoints() <= 0) {
                WinEvent("������� ������ �����\n");
                std::string str = "***����� ���� -> " + getTime() + "\n";
                EndGameEvent(str);
                WinPlayerOneEvent();
                return playerTwo;
            }
            else {
                WinEvent("������� ������ �����\n");
                std::string str = "***����� ���� -> " + getTime() + "\n";
                EndGameEvent(str);
                WinPlayerTwoEvent();
                return playerOne;
            }
        }
        else {
            std::cout << "�� ������� ������(��)" << std::endl;
        }
    }
};

void PrintMenu() {
    std::cout << "[1] - ������� ������� � �������� �� 100HP" << std::endl;
    std::cout << "[2] - ������� ������ �������" << std::endl;
    std::cout << "[3] - �������� ������ � �������" << std::endl;
    std::cout << "[4] - ������� ������ �� �������" << std::endl;
    std::cout << "[5] - ������ ����" << std::endl;
    std::cout << "[6] - ������� ���������� ��������� �������" << std::endl;
    std::cout << "[7] - �����" << std::endl;
}

int main() {
    setlocale(LC_ALL, "RU");
    srand(time(NULL));

    array<Room^>^ roomS = gcnew array<Room^>(100);
    Logger^ logger = gcnew Logger();
    int countRooms = 0;
    Statistic^ statistic = gcnew Statistic();

   while (true) {
       PrintMenu();
        int command;
        std::cin >> command;
        if (command == 1) {
            roomS[countRooms] = gcnew Room(100, countRooms);
            roomS[countRooms]->WinEvent += gcnew MessageDelegate(logger, &Logger::log);
            roomS[countRooms]->WinEvent += gcnew MessageDelegate(logger, &Logger::log);
            roomS[countRooms]->AddPlayerEvent += gcnew MessageDelegate(logger, &Logger::log);
            roomS[countRooms]->DeletePlayerEvent += gcnew MessageDelegate(logger, &Logger::log);
            roomS[countRooms]->StartGameEvent += gcnew MessageDelegate(logger, &Logger::log);
            roomS[countRooms]->EndGameEvent += gcnew MessageDelegate(logger, &Logger::log);
            roomS[countRooms]->WinPlayerOneEvent += gcnew ActionDelegate(statistic, &Statistic::winPlayerOne);
            roomS[countRooms]->WinPlayerTwoEvent += gcnew ActionDelegate(statistic, &Statistic::winPlayerTwo);

            std::cout << "�� ������� ������� � " << countRooms << " � �������� �� 100HP" << std::endl;
            countRooms++;
        }
        else if (command == 2) {
            roomS[countRooms] = gcnew Room(countRooms);
            roomS[countRooms]->WinEvent += gcnew MessageDelegate(logger, &Logger::log);
            roomS[countRooms]->WinEvent += gcnew MessageDelegate(logger, &Logger::log);
            roomS[countRooms]->AddPlayerEvent += gcnew MessageDelegate(logger, &Logger::log);
            roomS[countRooms]->DeletePlayerEvent += gcnew MessageDelegate(logger, &Logger::log);
            roomS[countRooms]->StartGameEvent += gcnew MessageDelegate(logger, &Logger::log);
            roomS[countRooms]->EndGameEvent += gcnew MessageDelegate(logger, &Logger::log);
            roomS[countRooms]->WinPlayerOneEvent += gcnew ActionDelegate(statistic, &Statistic::winPlayerOne);
            roomS[countRooms]->WinPlayerTwoEvent += gcnew ActionDelegate(statistic, &Statistic::winPlayerTwo);
            std::cout << "�� ������� ������ ������� � " << countRooms << std::endl;
            countRooms++;
        }
        else if (command == 3) {
            if (countRooms == 0) {
                std::cout << "��� ������" << std::endl;
                continue;
            }
            int idRoom = 0;
            int healthPoints = 0;
            int numberPlayer = 0;
            std::cout << "������� � ������� : ";
            std::cin >> idRoom;
            if (idRoom >= countRooms) {
                std::cout << "����� ������� �� ����������" << std::endl;
                continue;
            }
            std::cout << "������� � ������ [1] ��� [2] : ";
            std::cin >> numberPlayer;
            if (numberPlayer != 1 && numberPlayer != 2) {
                std::cout << "������ ������ �� ����������" << std::endl;
                continue;
            }
            std::cout << "������� ���������� HP ��� ������ : ";
            std::cin >> healthPoints;
            if (numberPlayer == 1) {
                roomS[idRoom]->addPlayerOne(gcnew Player(healthPoints));
            }
            else if (numberPlayer == 2) {
                roomS[idRoom]->addPlayerTwo(gcnew Player(healthPoints));
            }
        }
        else if (command == 4) {
            if (countRooms == 0) {
                std::cout << "��� ������" << std::endl;
                continue;
            }
            int idRoom = 0;
            int numberPlayer = 0;
            std::cout << "������� � ������� : ";
            std::cin >> idRoom;
            if (idRoom >= countRooms) {
                std::cout << "����� ������� �� ����������" << std::endl;
                continue;
            }
            std::cout << "������� � ������ [1] ��� [2] : ";
            std::cin >> numberPlayer;
            if (numberPlayer != 1 && numberPlayer != 2) {
                std::cout << "������ ������ �� ����������" << std::endl;
                continue;
            }
            if (numberPlayer == 1) {
                roomS[idRoom]->deletePlayerOne();
            }
            else if (numberPlayer == 2) {
                roomS[idRoom]->deletePlayerTwo();
            }
        }
        else if (command == 5) {
            if (countRooms == 0) {
                std::cout << "��� ������" << std::endl;
                continue;
            }
            
            int idRoom = 0;
            std::cout << "������� � ������� : ";
            std::cin >> idRoom;
            if (idRoom >= countRooms) {
                std::cout << "����� ������� �� ����������" << std::endl;
                continue;
            }
            Player^ winPlayer = roomS[idRoom]->Play();
            if (winPlayer == roomS[idRoom]->getPlayerOne()) std::cout << "������� ������ �����" << std::endl;
            else std::cout << "������� ������ �����" << std::endl;
        }
        else if (command == 6) {
            std::cout << "���������� ����� ������ �1 : " << statistic->getCountWinPlayerOne() << std::endl
                << "���������� ����� ������ �2 : " << statistic->getCountWinPlayerTwo() << std::endl;
        }
        else if (command == 7) {
            break;
        }
        else {
            std::cout << "����� ������� ���" << std::endl;
        }
   }
    return 0;
}
