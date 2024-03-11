//ä�� Ŭ���̾�Ʈ
#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h> //Winsock ������� include. WSADATA �������.
#include <WS2tcpip.h>
#include <string>
#include <sstream>
#include <iostream>
#include <thread>
#include <vector>

#include <stdio.h>
#include <Windows.h>
#include <mmsystem.h> //�Ҹ� ���
#pragma comment(lib,"winmm.lib") //�Ҹ� ���



#define MAX_SIZE 1024

using namespace std;

SOCKET client_sock;
SOCKADDR_IN client_addr = {};
string my_id, my_nick, my_pw, my_name, number, my_phonenumber, result;
string login_User_nick, login_User_id; //�α��� ���� 
string User_edit_pw; //�α����� ���� �̸� ���̵� ����
string edit_check = "N";

WSADATA wsa;
// Winsock�� �ʱ�ȭ�ϴ� �Լ�. MAKEWORD(2, 2)�� Winsock�� 2.2 ������ ����ϰڴٴ� �ǹ�..+
// ���࿡ �����ϸ� 0��, �����ϸ� �� �̿��� ���� ��ȯ.
// 0�� ��ȯ�ߴٴ� ���� Winsock�� ����� �غ� �Ǿ��ٴ� �ǹ�.
int code = WSAStartup(MAKEWORD(2, 2), &wsa);

void login(); // �α��� 
bool login_flag = false;
bool stop_flag = false;
bool findID_flag = false;
bool findPW_flag = false;
bool join_flag = false;
bool join_id_flag = false;
bool User_Edit_falg = false;
bool chat_list_flag = false;
bool dblist_flag = false;
bool chatstart = false;

void socket_init(); // �������� ����
void findID(); // ���̵� ã��
void findPW(); // �н����� ã��
void join();
int findID_recv();
int findPW_recv();
int join_recv();

int choice = 0;
int editchoice = 0;

void socket_init() {

    client_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP); // 

    // ������ ���� ���� ���� �κ�
    //SOCKADDR_IN client_addr = {};
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(7777);
    InetPton(AF_INET, TEXT("127.0.0.1"), &client_addr.sin_addr);
    //InetPton(AF_INET, TEXT("192.168.0.55"), &client_addr.sin_addr);

    while (1) {
        if (!connect(client_sock, (SOCKADDR*)&client_addr, sizeof(client_addr))) { // ���� ������ ������ �ش��ϴ� server�� ����!            
            cout << "***SYSTEM MESSAGE : Server Connect***" << endl;
            string msg = "auto produce";
            send(client_sock, msg.c_str(), msg.length(), 0); // ���ῡ �����ϸ� client �� �Է��� �г����� ������ ����
            //send(client_sock, my_pw.c_str(), my_pw.length(), 0);          

            break;
        }
        cout << "Connecting..." << endl;
    }
}
// ��Ȳ�� ���� ��µǴ� ȭ���
void intro()
{
    cout << "\n";
    system("cls");
    cout << "@ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @" << endl;
    cout << "@ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @" << endl;
    cout << "@ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @" << endl;
    cout << "@ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @" << endl;
    cout << "@ @ @ @ @ @         @ @ @ @ @         @ @ @ @ @ @" << endl;
    cout << "@ @ @ @                                   @ @ @ @" << endl;
    cout << "@ @ @ @                                   @ @ @ @" << endl;
    cout << "@ @ @                                       @ @ @" << endl;
    cout << "@ @ @                                       @ @ @" << endl;
    cout << "@ @                                           @ @" << endl;
    cout << "@ @                                           @ @" << endl;
    cout << "@ @             @ @           @ @             @ @" << endl;
    cout << "@             @ @ @           @ @ @             @" << endl;
    cout << "@             @ @ @ @       @ @ @ @             @" << endl;
    cout << "@             @ @ @           @ @ @             @" << endl;
    cout << "@                                               @" << endl;
    cout << "@                                               @" << endl;
    cout << "@                                               @" << endl;
    cout << "@             @                   @             @" << endl;
    cout << "@ @ @           @ @ @ @ @ @ @ @ @           @ @ @" << endl;
    cout << "@ @ @ @ @       @ @ @ @ @ @ @ @ @       @ @ @ @ @" << endl;
    cout << "@ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @" << endl;
    cout << "@ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @" << endl;
    cout << "@ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @" << endl;
    cout << "@ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @" << endl;


    Sleep(4000);
    system("cls");
}

void joinsuccessscreen()
{
    system("cls");


    cout << " "; cout << "                                                                 \n";
    cout << " "; cout << "           $ .           * $ .                                   \n";
    cout << " "; cout << "           .             . @ !         :                         \n";
    cout << " "; cout << "                           . @         = =     . #               \n";
    cout << " "; cout << "                             = -       : @       .               \n";
    cout << " "; cout << "                             $     @ ! ! ,                       \n";
    cout << " "; cout << "                       . - . @     # .                           \n";
    cout << " "; cout << "             ;       - @ ! @ =     : # $     ; :                 \n";
    cout << " "; cout << "             @ :     ! @ ~   * @ -   . $     @ @                 \n";
    cout << " "; cout << "             .       @ @ *     ; @ @ @ =     ~ ,                 \n";
    cout << " "; cout << "                   , @ @ @ .     = @ -               , @         \n";
    cout << " "; cout << "                   @ @ @ @ @ ,   - @ @ :             . ~         \n";
    cout << " "; cout << "                 : @ @ @ @ @ # .     - @ ,   , .                 \n";
    cout << " "; cout << "                 = @ @ @ @ @ @ $ .     ; # $ @ # = ~             \n";
    cout << " "; cout << "                 @ @ @ @ @ @ @ @ # , . # # *   - * # # .         \n";
    cout << " "; cout << "               $ @ @ @ @ @ @ @ @ @ @ , : . @       . ,           \n";
    cout << " "; cout << "             . @ @ @ @ @ @ @ @ @ @ @ @ = ~ $                     \n";
    cout << " "; cout << "             ~ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @                     \n";
    cout << " "; cout << "             # @ @ @ @ @ @ @ @ @ @ @ @ $ ; ,                     \n";
    cout << " "; cout << "           . @ @ @ @ @ @ @ @ @ @ @ @ .                           \n";
    cout << " "; cout << "           $ @ @ @ @ @ @ @ @ @ ! :                               \n";
    cout << " "; cout << "         - @ @ @ @ @ @ @ @ =                 @ :                 \n";
    cout << " "; cout << "         ! @ @ @ @ @ # : * ;                 ~ ,                 \n";
    cout << " "; cout << "         @ @ @ @ $ .     # $                                     \n";
    cout << " "; cout << "         ! @ * -         , ,                                     \n";
    cout << " "; cout << "                                                                 \n";
    cout << " "; cout << "                 ��ȸ�������� �����մϴ١�                       \n\n";
    cout << " "; cout << "�� " << my_name << " �� ȯ���մϴ�. ��" << endl;
    Sleep(4000);

}

void mainscreen()
{

    system("cls");
    cout << "\n";
    cout << " "; cout << "�����������������������������\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��    ����  ����  ����  ����   ��   ��  ��    ��\n";
    cout << " "; cout << "��    ��      ��  ��    ��    ��  ��   ��   ���      ��\n";
    cout << " "; cout << "��    ��      ��  ��    ��    ����   ��   ����    ��\n";
    cout << " "; cout << "��    ����  ����    ��    ��  ��   ��   ��  ��    ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��                   < ���� ȭ�� >                    ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��                    1. �� �� ��                     ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��                    2. id ã��                      ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��                    3. pw ã��                      ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��                    4. ȸ������                     ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "�����������������������������\n\n";
    cout << ">>";
    cin >> choice;

}

void findIDscreen()
{
    cout << "\n";
    cout << " "; cout << "�����������������������������\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��    ����  ����  ����  ����   ��   ��  ��    ��\n";
    cout << " "; cout << "��    ��      ��  ��    ��    ��  ��   ��   ���      ��\n";
    cout << " "; cout << "��    ��      ��  ��    ��    ����   ��   ����    ��\n";
    cout << " "; cout << "��    ����  ����    ��    ��  ��   ��   ��  ��    ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "�����������������������������\n";
    cout << " "; cout << "��======================ID ã��=======================��\n";
    cout << " "; cout << "��1.�̸� �Է�(10�ڸ� ����)                            ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��2.��ȭ��ȣ �Է� (- ����)                            ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "����� �� �ٽ� �Է�                                   ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��'exit' �Է� �� ���� �޴� ����                       ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "�����������������������������\n\n";
}

void findPWscreen()
{
    cout << "\n";
    cout << " "; cout << "�����������������������������\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��    ����  ����  ����  ����   ��   ��  ��    ��\n";
    cout << " "; cout << "��    ��      ��  ��    ��    ��  ��   ��   ���      ��\n";
    cout << " "; cout << "��    ��      ��  ��    ��    ����   ��   ����    ��\n";
    cout << " "; cout << "��    ����  ����    ��    ��  ��   ��   ��  ��    ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "�����������������������������\n";
    cout << " "; cout << "��======================PW ã��=======================��\n";
    cout << " "; cout << "��1.���̵� �Է�(10�ڸ� ����)                          ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��2.�̸� �Է�(10�ڸ� ����)                            ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��3.��ȭ��ȣ �Է� (- ����)                            ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "����� �� �ٽ� �Է�                                   ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��'exit' �Է� �� ���� �޴� ����                       ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "�����������������������������\n\n";
}

void joinscreen()
{
    cout << "\n";
    cout << " "; cout << "�����������������������������\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��    ����  ����  ����  ����   ��   ��  ��    ��\n";
    cout << " "; cout << "��    ��      ��  ��    ��    ��  ��   ��   ���      ��\n";
    cout << " "; cout << "��    ��      ��  ��    ��    ����   ��   ����    ��\n";
    cout << " "; cout << "��    ����  ����    ��    ��  ��   ��   ��  ��    ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "�����������������������������\n";
    cout << " "; cout << "��======================ȸ������======================��\n";
    cout << " "; cout << "��1.���ϴ� ���̵� �Է� & ���̵� �ߺ�üũ              ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��2.�̸� �Է�(���̵� �ߺ�üũ ��)                     ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��3.��й�ȣ �Է�                                     ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��4.��ȭ��ȣ �Է�                                     ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��5.�г��� �Է�                                       ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "�����������������������������\n\n";

}

void loginscreen()
{
    cout << "\n";
    cout << " "; cout << "�����������������������������\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��    ����  ����  ����  ����   ��   ��  ��    ��\n";
    cout << " "; cout << "��    ��      ��  ��    ��    ��  ��   ��   ���      ��\n";
    cout << " "; cout << "��    ��      ��  ��    ��    ����   ��   ����    ��\n";
    cout << " "; cout << "��    ����  ����    ��    ��  ��   ��   ��  ��    ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "�����������������������������\n";
    cout << " "; cout << "��=======================�α���=======================��\n";
    cout << " "; cout << "��1.���̵� �Է�                                       ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��2.��й�ȣ �Է�                                     ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "�����������������������������\n\n";

}

void userscreen()
{
    cout << "\n";
    cout << " "; cout << "�����������������������������\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��    ����  ����  ����  ����   ��   ��  ��    ��\n";
    cout << " "; cout << "��    ��      ��  ��    ��    ��  ��   ��   ���      ��\n";
    cout << " "; cout << "��    ��      ��  ��    ��    ����   ��   ����    ��\n";
    cout << " "; cout << "��    ����  ����    ��    ��  ��   ��   ��  ��    ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��                   < �α��� ���� >                  ��\n";
    cout << " "; cout << "��             ���Ͻô� �޴��� ������ �ּ���.         ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��                    1. ä���ϱ�                     ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��                    2. ȸ������ ����                ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��                    3. �α׾ƿ�                     ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��                    4. ����                         ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "�����������������������������\n\n";
    cin >> choice;
}

void edit_screen()
{
    cout << "\n";
    cout << " "; cout << "�����������������������������\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��    ����  ����  ����  ����   ��   ��  ��    ��\n";
    cout << " "; cout << "��    ��      ��  ��    ��    ��  ��   ��   ���      ��\n";
    cout << " "; cout << "��    ��      ��  ��    ��    ����   ��   ����    ��\n";
    cout << " "; cout << "��    ����  ����    ��    ��  ��   ��   ��  ��    ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��                 < ȸ������ ���� >                  ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��                    1. PW ����                      ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��                    2. �г��� ����                  ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "��                                                    ��\n";
    cout << " "; cout << "�����������������������������\n\n";
    cin >> editchoice;
}

void editpwscreen()
{
    cout << "!��й�ȣ ����!" << endl;
}

void edit_Menu()
{
    system("cls");
    cout << "��й�ȣ Ȯ�οϷ�" << endl;

}
//-------------------------------//

//������ �α��� ���� ���� �ޱ�, ���� ä�� �ޱ�
int chat_recv()
{
    char buf[MAX_SIZE] = { };
    string msg;

    while (chatstart) {
        ZeroMemory(&buf, MAX_SIZE);
        if (recv(client_sock, buf, MAX_SIZE, 0) > 0) {
            msg = buf;
            std::stringstream ss(msg);  // ���ڿ��� ��Ʈ��ȭ
            string user;
            ss >> user; // ��Ʈ���� ����, ���ڿ��� ���� �и��� ������ �Ҵ�. ���� ����� �̸��� user�� �����.
            if (user != login_User_nick) cout << buf << endl; // ���� ���� �� �ƴ� ��쿡�� ����ϵ���.
        }
        else {
            cout << "Server Off" << endl;
            return -1;
        }
    }

    while (!login_flag)
    {
        ZeroMemory(&buf, MAX_SIZE);

        if (recv(client_sock, buf, MAX_SIZE, 0) > 0) {

            // ���ڿ��� ��Ʈ���� �ְ� ������ �������� �����Ͽ� ���Ϳ� ����
            std::istringstream iss(buf);
            std::vector<std::string> tokens;
            std::string token;

            while (iss >> token) {
                tokens.push_back(token);
            }
            //�����κ��� ���� �޽��� ����
            // ( [0] : ��û ��� (1=�α��� ��) / [1] : ���� ��� ( �ظ��ؼ� "server") / [2] : ����� (�̰ɷ� �α��� ���� ����) 
            // / [3] : �޴� ��� / [4] ��� ���� �̸� ) / [5] ��� ���̵�
            if (tokens[1] != my_nick) {
                if (tokens[1] == "server") { // �����κ��� ���� �޽����� 
                    if (tokens[0] == "1") {
                        result = tokens[2];
                        if (result == "1") {
                            login_User_nick = tokens[4];
                            login_User_id = tokens[5];
                            cout << endl << " ============================================ " << endl;
                            cout << " �� " << login_User_nick << " �� �α��� �Ǿ����ϴ�." << endl;
                            cout << " �� 3�� �ڿ� ���� ȭ������ ���ϴ�." << endl;
                            Sleep(3000);
                            login_flag = true; //�̰� �ؾ��� �α��� ���� ������ �� ����.
                            break;
                        }
                        else {
                            cout << endl << " ============================================ " << endl;
                            cout << " �� �α��� ����" << endl;
                            cout << " �� 3�� �ڿ� ���� ȭ������ ���ϴ�." << endl;
                            Sleep(3000);
                            stop_flag = true;
                            break;
                        }
                    }
                    else {
                        cout << endl << " ============================================ " << endl;
                        cout << " �� �α��� ����" << endl;
                        cout << " �� 3�� �ڿ� ���� ȭ������ ���ϴ�." << endl;
                        Sleep(3000);
                        stop_flag = true;
                        break;
                    }
                }
                else {
                    cout << buf << endl;
                }
            }

            if (tokens[1] == login_User_nick) {
                cout << endl << " ============================================ " << endl;
                cout << " �� " << login_User_nick << " �� �α��� �Ǿ����ϴ�." << endl;
                cout << " �� 3�� �ڿ� ���� ȭ������ ���ϴ�." << endl;
                Sleep(3000);
                login_flag = true; //�̰� �ؾ��� �α��� ���� ������ �� ����.
                break;
            }
        }
        else {
            cout << "Server Off" << endl;
            return -1;
        }
    }
}
//ID ã��
void findID()
{
    system("cls");
    findIDscreen();

    while (!code) {

        if (findID_flag == true) { break; }

        if (stop_flag == true) { break; }

        string User_request = "2"; // ID ã�� ��ȣ?
        cout << " �̸��� �Է��� �ּ���. >> ";
        cin >> my_name;

        if (my_name == "exit") { // exit �Է� �� ����ȭ��
            stop_flag = true;
            break;
        }

        while (true) {
            cout << " ��ȭ��ȣ�� �Է��� �ּ���(- ����) >> ";
            cin >> number;

            if (number.length() != 11) {
                cout << endl << " ============================================ " << endl;
                cout << " �� ��ȭ��ȣ�� �ٽ� �Է����ּ���." << endl;
                continue;
            }
            break;
        }

        string first = number.substr(0, 3);
        string second = number.substr(3, 4);
        string third = number.substr(7, 4);
        my_phonenumber = first + "-" + second + "-" + third;
        string msg = User_request + " " + my_name + " " + my_phonenumber;
        send(client_sock, msg.c_str(), msg.length(), 0);

        std::thread th2(findID_recv);

        th2.join();
    }

}
//PW ã��
void findPW()
{
    system("cls");
    findPWscreen();

    while (!code) {

        if (findPW_flag == true) { break; }

        if (stop_flag == true) { break; }

        string User_request = "3"; // PW ã�� ��ȣ?
        cout << " ID �Է� >> ";
        cin >> my_id;

        if (my_id == "exit") {
            stop_flag == true;
            break;
        }

        cout << " �̸� �Է� >> ";
        cin >> my_name;

        while (true) {
            cout << " ��ȭ��ȣ �Է�(- ����) >> ";
            cin >> number;

            if (number.length() != 11) {
                cout << endl << " ============================================ " << endl;
                cout << " �� ��ȭ��ȣ�� �ٽ� �Է����ּ���." << endl;
                continue;
            }
            break;
        }

        string first = number.substr(0, 3);
        string second = number.substr(3, 4);
        string third = number.substr(7, 4);
        my_phonenumber = first + "-" + second + "-" + third;
        string msg = User_request + " " + my_id + " " + my_name + " " + my_phonenumber;
        send(client_sock, msg.c_str(), msg.length(), 0);

        std::thread th2(findPW_recv);

        th2.join();
    }


}
//������ ���� IDã�� ����� �ޱ�
int findID_recv() {
    char buf[MAX_SIZE] = { };
    string msg;

    while (1) {
        ZeroMemory(&buf, MAX_SIZE);
        if (recv(client_sock, buf, MAX_SIZE, 0) > 0) {

            // ���ڿ��� ��Ʈ���� �ְ� ������ �������� �����Ͽ� ���Ϳ� ����
            std::istringstream iss(buf);
            std::vector<std::string> tokens;
            std::string token;

            while (iss >> token) {
                tokens.push_back(token);
            }

            // ( [0] : ��û ��� (1=�α��� ��) / [1] : ���� ��� ( �ظ��ؼ� "server") / [2] : ����� (ID ã�� ���� ����) / [3] : �޴� ��� / [4] : ���(ã�� ID) )
            if (tokens[1] == "server") { // �����κ��� ���� �޽����� 
                if (tokens[0] == "2") {
                    result = tokens[2];
                    if (result == "1") {
                        cout << endl << " ============================================ " << endl;
                        cout << " �� ���̵� ã�� ����!" << endl;
                        string find_User_id = tokens[4];
                        cout << " �� " << my_name << " ���� ���̵�� : " << find_User_id << " �Դϴ�." << endl;
                        cout << " �� 3�� �� ���� ȭ������ ���ư��ϴ�." << endl;
                        findID_flag = true;
                        Sleep(3000);
                        break;
                        // ���⿡�� ���(result)�� ����ϰų� ó��
                    }
                    else {
                        findID_flag = false; // ID ã�� ���� ����
                        cout << endl << " ============================================ " << endl;
                        cout << " �� ID ã�� ����. �Է��� ������ �ٽ� Ȯ�����ּ���." << endl;
                        Sleep(4000);
                        break;
                    }
                }
                else {
                    findID_flag = false; // ID ã�� ���� ����
                    cout << endl << " ============================================ " << endl;
                    cout << " �� ID ã�� ����. �Է��� ������ �ٽ� Ȯ�����ּ���." << endl;
                    Sleep(4000);
                    break;
                }
            }
            else {
                cout << buf << endl;
            }
        }
    }
}
//������ ���� PWã�� ����� �ޱ�
int findPW_recv() {
    char buf[MAX_SIZE] = { };
    string msg;

    while (1) {
        ZeroMemory(&buf, MAX_SIZE);
        if (recv(client_sock, buf, MAX_SIZE, 0) > 0) {

            // ���ڿ��� ��Ʈ���� �ְ� ������ �������� �����Ͽ� ���Ϳ� ����
            std::istringstream iss(buf);
            std::vector<std::string> tokens;
            std::string token;

            while (iss >> token) {
                tokens.push_back(token);
            }

            // ( [0] : ��û ��� (1=�α��� ��) / [1] : ���� ��� ( �ظ��ؼ� "server") / [2] : ����� (ID ã�� ���� ����) / [3] : �޴� ��� / [4] : ���(ã�� ID) )
            if (tokens[1] == "server") { // �����κ��� ���� �޽����� 
                if (tokens[0] == "3") {
                    result = tokens[2];
                    if (result == "1") {
                        cout << endl << " ============================================ " << endl;
                        cout << " �� ��й�ȣ ã�� ����!" << endl;
                        string find_User_pw = tokens[4];
                        cout << " �� " << my_name << " ���� ��й�ȣ�� : " << find_User_pw << " �Դϴ�." << endl;
                        cout << " �� 3�� �� ���� ȭ������ ���ư��ϴ�." << endl;
                        findPW_flag = true;
                        Sleep(3000);
                        break;
                        // ���⿡�� ���(result)�� ����ϰų� ó��
                    }
                    else {
                        findPW_flag = false; // ID ã�� ���� ����
                        cout << endl << " ============================================ " << endl;
                        cout << " �� PW ã�� ����. �Է��� ������ �ٽ� Ȯ�����ּ���." << endl;
                        Sleep(4000);
                        break;
                    }
                }
                else {
                    findPW_flag = false; // ID ã�� ���� ����
                    cout << endl << " ============================================ " << endl;
                    cout << " �� PW ã�� ����. �Է��� ������ �ٽ� Ȯ�����ּ���." << endl;
                    Sleep(4000);
                    break;
                }
            }
            else {
                cout << buf << endl;
            }

        }
    }
}
//�α���
void login()
{
    while (1)
    {
        system("cls");
        loginscreen();

        // �̰� �����ؾ� �ٽ� �α��νõ� ����. 
        if (login_flag == true) { break; }


        string User_input;
        string User_request = "1";
        cout << " ���̵� �Է� >> ";
        cin >> User_input;
        my_nick = User_input;
        cout << " ��й�ȣ �Է� >> ";
        cin >> User_input;
        my_pw = User_input;


        string msg = User_request + " " + my_nick + " " + my_pw;
        send(client_sock, msg.c_str(), msg.length(), 0);


        std::thread th2(chat_recv);

        th2.join();
    }
}
//ȸ������
void join()
{
    system("cls");
    joinscreen();




    while (!code) {

        if (join_flag == true) { break; }

        // ID�Է�

        if (join_id_flag == false) {
            while (1)
            {
                cout << "���Ͻô� ID�� �Է��ϼ���. (10�ڸ�)>> ";
                cin >> my_id;

                if (my_id.length() > 10) {
                    cout << endl << "========================" << endl;
                    cout << " �� �Է� ������ �ڸ����� �Ѿ����ϴ�. �ٽ� �Է����ּ���. ";
                    continue;
                }
                else {
                    break;
                }
            }
            string User_request = "41"; // ȸ������ �� ���̵� Ȯ�� ��û ��ȣ
            string msg_id = User_request + " " + my_id;
            send(client_sock, msg_id.c_str(), msg_id.length(), 0);
        }

        else if (join_id_flag == true) {
            while (1) {
                cout << " �̸��� �Է��ϼ���. >> ";
                cin >> my_name;

                if (my_name.length() > 10) {
                    cout << endl << " ============================================ " << endl;
                    cout << " �� �Է� ������ �ڸ����� �Ѿ����ϴ�. �ٽ� �Է����ּ���. ";
                    continue;
                }
                else { break; }
            }

            while (1) {
                cout << " ��й�ȣ�� �Է��ϼ���. >> ";
                cin >> my_pw;

                if (my_pw.length() > 10) {
                    cout << endl << " ============================================ " << endl;
                    cout << " �� �Է� ������ �ڸ����� �Ѿ����ϴ�. �ٽ� �Է����ּ���. ";
                    continue;
                }
                else { break; }
            }

            while (1) {
                cout << " ��ȭ��ȣ�� �Է��ϼ���. >> ";
                cin >> number;

                if (number.length() > 11) {
                    cout << endl << " ============================================ " << endl;
                    cout << " �� �߸� �Է��ϼ̽��ϴ�. �ٽ� �Է����ּ���. ";
                    continue;
                }
                else { break; }
            }

            while (1) {
                cout << " �г����� �Է��ϼ���. >> ";
                cin >> my_nick;

                if (my_nick.length() > 10) {
                    cout << endl << " ============================================ " << endl;
                    cout << " �� �Է� ������ �ڸ����� �Ѿ����ϴ�. �ٽ� �Է����ּ���. ";
                    continue;
                }
                else { break; }
            }


            string first = number.substr(0, 3);
            string second = number.substr(3, 4);
            string third = number.substr(7, 4);
            my_phonenumber = first + "-" + second + "-" + third;
            string User_request = "4"; // ȸ������ ��û ��ȣ
            string msg = User_request + " " + my_id + " " + my_name + " " + my_pw + " " + my_phonenumber + " " + my_nick; // ģ�� ����� ����?
            send(client_sock, msg.c_str(), msg.length(), 0);

        }

        std::thread th(join_recv);

        th.join();
    }
}
//������ ���� ȸ������ ����� �ޱ�
int join_recv()
{
    char buf[MAX_SIZE] = { };
    string msg;


    while (1) {
        ZeroMemory(&buf, MAX_SIZE);
        if (recv(client_sock, buf, MAX_SIZE, 0) > 0) {

            // ���ڿ��� ��Ʈ���� �ְ� ������ �������� �����Ͽ� ���Ϳ� ����
            std::istringstream iss(buf);
            std::vector<std::string> tokens;
            std::string token;

            while (iss >> token) {
                tokens.push_back(token);
            }

            // ( [0] : ��û ��� (1=�α��� ��) / [1] : ���� ��� ( �ظ��ؼ� "server") / [2] : ����� (ȸ������ ���� ����) / [3] : �޴� ��� / [4] : ���� �̸� )
            if (tokens[1] == "server") {
                if (tokens[0] == "4") {
                    result = tokens[2];
                    if (result == "1") {
                        cout << endl << " ============================================ " << endl;
                        cout << " �� ȸ�� ���� ����!" << endl;
                        //cout << " �� " << my_name << " �� ȯ���մϴ�. " << endl;
                        //cout << " �� 4�� �� ���� ȭ������ ���ư��ϴ�. " << endl;
                        join_flag = true; // ȸ������ ���� ����
                        joinsuccessscreen();
                        //Sleep(4000);
                        break;
                    }
                    else if (result == "3") {
                        join_flag = false;
                        cout << endl << " ============================================ " << endl;
                        cout << " �� �Է��� ���̵� �������� �ʽ��ϴ�." << endl;
                        cout << " �� ȸ�������� �����մϴ�." << endl;
                        cout << " ============================================ " << endl;
                        join_id_flag = true;
                        Sleep(4000);
                        break;
                    }
                    else if (result == "4") {
                        cout << endl << " ============================================ " << endl;
                        cout << " �� ȸ������ ����! �̹� �����ϴ� ���̵��Դϴ�. " << endl;
                        join_flag = false;
                        join_id_flag = false;
                        Sleep(4000);
                        join();
                        break;
                    }
                    else {
                        join_flag = false; // ȸ������ ���� ����
                        cout << endl << " ============================================ " << endl;
                        cout << " �� ȸ������ ����! �Է��� ������ �ٽ� Ȯ�����ּ���. " << endl;
                        Sleep(4000);
                        join();
                        break;
                    }
                }
                else {
                    join_flag = false; // ȸ������ ���� ����
                    cout << endl << " ============================================ " << endl;
                    cout << " �� ȸ������ ����! �Է��� ������ �ٽ� Ȯ�����ּ���. " << endl;
                    Sleep(4000);
                    join();
                    break;
                }
            }


        }
    }
}
//ä�ù� ����& Ŭ���̾�Ʈ���� �Է��� ä�� ������ ������
void chatting()
{
    std::thread th2(chat_recv);
    system("cls");
    string User_request = "5";          // ������ User_request = "5"�� �����Ͽ� ���� ������ ���� ����
    send(client_sock, User_request.c_str(), User_request.length(), 0);



    while (1) {
        string User_request = "51";         // ä��
        string user_msg;
        getline(cin, user_msg); // ����� �Է��� �� �ٷ� �б�

        string msg = User_request + " " + user_msg;
        send(client_sock, msg.c_str(), msg.length(), 0);

    }

    th2.join();

}
//������ ���� PW ���� ����� �ޱ�
int edit_recv() {
    string msg;

    while (1) {
        char buf[MAX_SIZE] = { };
        ZeroMemory(&buf, MAX_SIZE);
        if (recv(client_sock, buf, MAX_SIZE, 0) > 0) {

            // ���ڿ��� ��Ʈ���� �ְ� ������ �������� �����Ͽ� ���Ϳ� ����
            std::istringstream iss(buf);
            std::vector<std::string> tokens;
            std::string token;

            while (iss >> token) {
                tokens.push_back(token);
            }

            // ( [0] : ��û ��� (1=�α��� ��) / [1] : ���� ��� ( �ظ��ؼ� "server") / [2] : ����� (ID ã�� ���� ����) / [3] : �޴� ��� / [4] : ���(ã�� ID) )
            if (tokens[1] == "server") { // �����κ��� ���� �޽����� 
                if (tokens[0] == "8") {
                    result = tokens[2];
                    if (result == "1") {
                        cout << endl << " ============================================ " << endl;
                        cout << " �� �Է��Ͻ� ��й�ȣ�� ��ġ�մϴ�." << endl;
                        cout << " �� 3�� �� ���� �������� �Ѿ�ϴ�." << endl;
                        Sleep(3000);
                        system("cls");
                        edit_Menu();

                        cout << " ============================================ " << endl;
                        cout << " �� �����Ͻ� ��й�ȣ�� �Է��ϼ���. >> ";
                        string User_input;
                        string User_request = "8";
                        edit_check = "Y";

                        cin >> User_input;
                        User_edit_pw = User_input;

                        while (1) {
                            string msg = User_request + " " + User_edit_pw + " " + login_User_id + " " + edit_check;
                            send(client_sock, msg.c_str(), msg.length(), 0);
                            break;
                        }
                        // ���⿡�� ���(result)�� ����ϰų� ó��
                    }

                    else if (result == "2") {
                        cout << endl << " ============================================ " << endl;
                        cout << " �� ��й�ȣ�� �߸� �Է��ϼ̽��ϴ�. " << endl;

                        Sleep(4000);
                        break;
                        /*User_Edit();*/

                    }
                    else if (result == "3") {
                        cout << endl << " ============================================ " << endl;
                        cout << " �� ��й�ȣ�� ����Ǿ����ϴ�." << endl;
                        User_Edit_falg = true;
                        tokens.clear();
                        Sleep(4000);
                        break;
                    }
                    else {
                        cout << endl << " ============================================ " << endl;
                        cout << " �� �������." << endl;
                        Sleep(4000);
                    }
                }
                else {
                    cout << endl << " ============================================ " << endl;
                    cout << " �� �������." << endl;
                    Sleep(4000);
                    /*User_Edit();*/
                }
            }
            else {
                cout << buf << endl;
            }
        }
    }
}
//PW ����
void edit_pw()
{
    edit_check = "N";
    while (edit_check == "N") {
        system("cls");
        editpwscreen();

        //edit_check �� �⺻�� N
        string User_input;
        string User_request = "8";

        cout << " ============================================ " << endl;
        cout << " " << login_User_nick << " ���� ��й�ȣ ���� ������" << endl;
        cout << " [����] ���� ��й�ȣ�� �Է��ϼ���. >> ";
        cin >> User_input;
        User_edit_pw = User_input;

        std::thread th2(edit_recv);

        string msg = User_request + " " + User_edit_pw + " " + login_User_id + " " + edit_check;
        send(client_sock, msg.c_str(), msg.length(), 0);



        th2.join();
    }
}

//�г����� ���� �̿ϼ�
string change_nick;

void editnickscreen()
{
    cout << "���� �г��� : " << login_User_nick;
    cout << "�����Ͻ� �г����� �Է����ּ��� :";



}

void edit_nick()
{
    while (edit_check == "N") {
        system("cls");
        editnickscreen();

        cin >> change_nick;

        string User_request = "9";

        while (1) {
            string msg = User_request + " " + change_nick + " " + edit_check;
            send(client_sock, msg.c_str(), msg.length(), 0);
            break;
        }
        std::thread th2(edit_recv);

        th2.join();
    }
}

//ȸ������ ����
void edit_user()
{
    while (1) {
        system("cls");
        edit_screen();

        if (editchoice == 1) { // pw����
            edit_pw();
            break;
        }
        else if (editchoice == 2) { // �г��� ����
            edit_nick();
            break;
        }
        else if (editchoice == 3) { // ����
            break;
        }
        else {
            cout << "�߸��� �Է��Դϴ�. �ٽ� �Է����ּ���";
        }
    }

}


int main()
{
    intro();
    socket_init();

    while (1)
    {
        if (login_flag == false) {


            mainscreen();
            stop_flag = false;
            findID_flag = false;
            findPW_flag = false;
            join_id_flag = false;
            join_flag = false;

            if (choice == 1) { // �α���
                login();
            }
            else if (choice == 2) { // idã��
                findID();
            }
            else if (choice == 3) { // pwã��
                findPW();
            }
            else if (choice == 4) { // ȸ������
                join();
            }
            else { // ����
                cout << "!!" << endl << endl;
                WSACleanup();
                return 0;
            }
            system("cls");
        }

        else {
            userscreen();

            if (choice == 1) { // ä���ϱ�
                chatstart = true;
                chatting();

            }
            else if (choice == 2) { // ȸ������ ����
                edit_user();
            }
            else if (choice == 3) { //�α׾ƿ�

                login_flag = false;
            }

            else if (choice == 4) { // ����
                cout << "!!" << endl << endl;
                WSACleanup();
                return 0;
            }
            system("cls");

        }


    }
}


