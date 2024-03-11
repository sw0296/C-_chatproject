//채팅 클라이언트
#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h> //Winsock 헤더파일 include. WSADATA 들어있음.
#include <WS2tcpip.h>
#include <string>
#include <sstream>
#include <iostream>
#include <thread>
#include <vector>

#include <stdio.h>
#include <Windows.h>
#include <mmsystem.h> //소리 출력
#pragma comment(lib,"winmm.lib") //소리 출력



#define MAX_SIZE 1024

using namespace std;

SOCKET client_sock;
SOCKADDR_IN client_addr = {};
string my_id, my_nick, my_pw, my_name, number, my_phonenumber, result;
string login_User_nick, login_User_id; //로그인 정보 
string User_edit_pw; //로그인한 유저 이름 아이디 저장
string edit_check = "N";

WSADATA wsa;
// Winsock를 초기화하는 함수. MAKEWORD(2, 2)는 Winsock의 2.2 버전을 사용하겠다는 의미..+
// 실행에 성공하면 0을, 실패하면 그 이외의 값을 반환.
// 0을 반환했다는 것은 Winsock을 사용할 준비가 되었다는 의미.
int code = WSAStartup(MAKEWORD(2, 2), &wsa);

void login(); // 로그인 
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

void socket_init(); // 소켓정보 저장
void findID(); // 아이디 찾기
void findPW(); // 패스워드 찾기
void join();
int findID_recv();
int findPW_recv();
int join_recv();

int choice = 0;
int editchoice = 0;

void socket_init() {

    client_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP); // 

    // 연결할 서버 정보 설정 부분
    //SOCKADDR_IN client_addr = {};
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(7777);
    InetPton(AF_INET, TEXT("127.0.0.1"), &client_addr.sin_addr);
    //InetPton(AF_INET, TEXT("192.168.0.55"), &client_addr.sin_addr);

    while (1) {
        if (!connect(client_sock, (SOCKADDR*)&client_addr, sizeof(client_addr))) { // 위에 설정한 정보에 해당하는 server로 연결!            
            cout << "***SYSTEM MESSAGE : Server Connect***" << endl;
            string msg = "auto produce";
            send(client_sock, msg.c_str(), msg.length(), 0); // 연결에 성공하면 client 가 입력한 닉네임을 서버로 전송
            //send(client_sock, my_pw.c_str(), my_pw.length(), 0);          

            break;
        }
        cout << "Connecting..." << endl;
    }
}
// 상황에 따라 출력되는 화면들
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
    cout << " "; cout << "                 ★회원가입을 축하합니다★                       \n\n";
    cout << " "; cout << "※ " << my_name << " 님 환영합니다. ※" << endl;
    Sleep(4000);

}

void mainscreen()
{

    system("cls");
    cout << "\n";
    cout << " "; cout << "■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■    ■■■  ■■■  ■■■  ■■■   ■   ■  ■    ■\n";
    cout << " "; cout << "■    ■      ■  ■    ■    ■  ■   ■   ■■      ■\n";
    cout << " "; cout << "■    ■      ■  ■    ■    ■■■   ■   ■■■    ■\n";
    cout << " "; cout << "■    ■■■  ■■■    ■    ■  ■   ■   ■  ■    ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■                   < 시작 화면 >                    ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■                    1. 로 그 인                     ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■                    2. id 찾기                      ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■                    3. pw 찾기                      ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■                    4. 회원가입                     ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n\n";
    cout << ">>";
    cin >> choice;

}

void findIDscreen()
{
    cout << "\n";
    cout << " "; cout << "■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■    ■■■  ■■■  ■■■  ■■■   ■   ■  ■    ■\n";
    cout << " "; cout << "■    ■      ■  ■    ■    ■  ■   ■   ■■      ■\n";
    cout << " "; cout << "■    ■      ■  ■    ■    ■■■   ■   ■■■    ■\n";
    cout << " "; cout << "■    ■■■  ■■■    ■    ■  ■   ■   ■  ■    ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n";
    cout << " "; cout << "■======================ID 찾기=======================■\n";
    cout << " "; cout << "■1.이름 입력(10자리 이하)                            ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■2.전화번호 입력 (- 제외)                            ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■실패 시 다시 입력                                   ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■'exit' 입력 시 메인 메뉴 복귀                       ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n\n";
}

void findPWscreen()
{
    cout << "\n";
    cout << " "; cout << "■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■    ■■■  ■■■  ■■■  ■■■   ■   ■  ■    ■\n";
    cout << " "; cout << "■    ■      ■  ■    ■    ■  ■   ■   ■■      ■\n";
    cout << " "; cout << "■    ■      ■  ■    ■    ■■■   ■   ■■■    ■\n";
    cout << " "; cout << "■    ■■■  ■■■    ■    ■  ■   ■   ■  ■    ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n";
    cout << " "; cout << "■======================PW 찾기=======================■\n";
    cout << " "; cout << "■1.아이디 입력(10자리 이하)                          ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■2.이름 입력(10자리 이하)                            ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■3.전화번호 입력 (- 제외)                            ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■실패 시 다시 입력                                   ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■'exit' 입력 시 메인 메뉴 복귀                       ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n\n";
}

void joinscreen()
{
    cout << "\n";
    cout << " "; cout << "■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■    ■■■  ■■■  ■■■  ■■■   ■   ■  ■    ■\n";
    cout << " "; cout << "■    ■      ■  ■    ■    ■  ■   ■   ■■      ■\n";
    cout << " "; cout << "■    ■      ■  ■    ■    ■■■   ■   ■■■    ■\n";
    cout << " "; cout << "■    ■■■  ■■■    ■    ■  ■   ■   ■  ■    ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n";
    cout << " "; cout << "■======================회원가입======================■\n";
    cout << " "; cout << "■1.원하는 아이디 입력 & 아이디 중복체크              ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■2.이름 입력(아이디 중복체크 후)                     ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■3.비밀번호 입력                                     ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■4.전화번호 입력                                     ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■5.닉네임 입력                                       ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n\n";

}

void loginscreen()
{
    cout << "\n";
    cout << " "; cout << "■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■    ■■■  ■■■  ■■■  ■■■   ■   ■  ■    ■\n";
    cout << " "; cout << "■    ■      ■  ■    ■    ■  ■   ■   ■■      ■\n";
    cout << " "; cout << "■    ■      ■  ■    ■    ■■■   ■   ■■■    ■\n";
    cout << " "; cout << "■    ■■■  ■■■    ■    ■  ■   ■   ■  ■    ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n";
    cout << " "; cout << "■=======================로그인=======================■\n";
    cout << " "; cout << "■1.아이디 입력                                       ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■2.비밀번호 입력                                     ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n\n";

}

void userscreen()
{
    cout << "\n";
    cout << " "; cout << "■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■    ■■■  ■■■  ■■■  ■■■   ■   ■  ■    ■\n";
    cout << " "; cout << "■    ■      ■  ■    ■    ■  ■   ■   ■■      ■\n";
    cout << " "; cout << "■    ■      ■  ■    ■    ■■■   ■   ■■■    ■\n";
    cout << " "; cout << "■    ■■■  ■■■    ■    ■  ■   ■   ■  ■    ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■                   < 로그인 성공 >                  ■\n";
    cout << " "; cout << "■             원하시는 메뉴를 선택해 주세요.         ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■                    1. 채팅하기                     ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■                    2. 회원정보 변경                ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■                    3. 로그아웃                     ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■                    4. 종료                         ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n\n";
    cin >> choice;
}

void edit_screen()
{
    cout << "\n";
    cout << " "; cout << "■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■    ■■■  ■■■  ■■■  ■■■   ■   ■  ■    ■\n";
    cout << " "; cout << "■    ■      ■  ■    ■    ■  ■   ■   ■■      ■\n";
    cout << " "; cout << "■    ■      ■  ■    ■    ■■■   ■   ■■■    ■\n";
    cout << " "; cout << "■    ■■■  ■■■    ■    ■  ■   ■   ■  ■    ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■                 < 회원정보 수정 >                  ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■                    1. PW 변경                      ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■                    2. 닉네임 변경                  ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■                                                    ■\n";
    cout << " "; cout << "■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n\n";
    cin >> editchoice;
}

void editpwscreen()
{
    cout << "!비밀번호 변경!" << endl;
}

void edit_Menu()
{
    system("cls");
    cout << "비밀번호 확인완료" << endl;

}
//-------------------------------//

//서버로 로그인 성공 실패 받기, 상대방 채팅 받기
int chat_recv()
{
    char buf[MAX_SIZE] = { };
    string msg;

    while (chatstart) {
        ZeroMemory(&buf, MAX_SIZE);
        if (recv(client_sock, buf, MAX_SIZE, 0) > 0) {
            msg = buf;
            std::stringstream ss(msg);  // 문자열을 스트림화
            string user;
            ss >> user; // 스트림을 통해, 문자열을 공백 분리해 변수에 할당. 보낸 사람의 이름만 user에 저장됨.
            if (user != login_User_nick) cout << buf << endl; // 내가 보낸 게 아닐 경우에만 출력하도록.
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

            // 문자열을 스트림에 넣고 공백을 기준으로 분할하여 벡터에 저장
            std::istringstream iss(buf);
            std::vector<std::string> tokens;
            std::string token;

            while (iss >> token) {
                tokens.push_back(token);
            }
            //서버로부터 오는 메시지 형태
            // ( [0] : 요청 결과 (1=로그인 등) / [1] : 보낸 사람 ( 왠만해선 "server") / [2] : 결과값 (이걸로 로그인 성공 여부) 
            // / [3] : 받는 사람 / [4] 디비 유저 이름 ) / [5] 디비 아이디
            if (tokens[1] != my_nick) {
                if (tokens[1] == "server") { // 서버로부터 오는 메시지인 
                    if (tokens[0] == "1") {
                        result = tokens[2];
                        if (result == "1") {
                            login_User_nick = tokens[4];
                            login_User_id = tokens[5];
                            cout << endl << " ============================================ " << endl;
                            cout << " ※ " << login_User_nick << " 님 로그인 되었습니다." << endl;
                            cout << " ※ 3초 뒤에 메인 화면으로 갑니다." << endl;
                            Sleep(3000);
                            login_flag = true; //이걸 해야지 로그인 여부 결정할 수 있음.
                            break;
                        }
                        else {
                            cout << endl << " ============================================ " << endl;
                            cout << " ※ 로그인 실패" << endl;
                            cout << " ※ 3초 뒤에 메인 화면으로 갑니다." << endl;
                            Sleep(3000);
                            stop_flag = true;
                            break;
                        }
                    }
                    else {
                        cout << endl << " ============================================ " << endl;
                        cout << " ※ 로그인 실패" << endl;
                        cout << " ※ 3초 뒤에 메인 화면으로 갑니다." << endl;
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
                cout << " ※ " << login_User_nick << " 님 로그인 되었습니다." << endl;
                cout << " ※ 3초 뒤에 메인 화면으로 갑니다." << endl;
                Sleep(3000);
                login_flag = true; //이걸 해야지 로그인 여부 결정할 수 있음.
                break;
            }
        }
        else {
            cout << "Server Off" << endl;
            return -1;
        }
    }
}
//ID 찾기
void findID()
{
    system("cls");
    findIDscreen();

    while (!code) {

        if (findID_flag == true) { break; }

        if (stop_flag == true) { break; }

        string User_request = "2"; // ID 찾기 번호?
        cout << " 이름을 입력해 주세요. >> ";
        cin >> my_name;

        if (my_name == "exit") { // exit 입력 시 메인화면
            stop_flag = true;
            break;
        }

        while (true) {
            cout << " 전화번호를 입력해 주세요(- 제외) >> ";
            cin >> number;

            if (number.length() != 11) {
                cout << endl << " ============================================ " << endl;
                cout << " ※ 전화번호를 다시 입력해주세요." << endl;
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
//PW 찾기
void findPW()
{
    system("cls");
    findPWscreen();

    while (!code) {

        if (findPW_flag == true) { break; }

        if (stop_flag == true) { break; }

        string User_request = "3"; // PW 찾기 번호?
        cout << " ID 입력 >> ";
        cin >> my_id;

        if (my_id == "exit") {
            stop_flag == true;
            break;
        }

        cout << " 이름 입력 >> ";
        cin >> my_name;

        while (true) {
            cout << " 전화번호 입력(- 제외) >> ";
            cin >> number;

            if (number.length() != 11) {
                cout << endl << " ============================================ " << endl;
                cout << " ※ 전화번호를 다시 입력해주세요." << endl;
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
//서버로 부터 ID찾기 결과값 받기
int findID_recv() {
    char buf[MAX_SIZE] = { };
    string msg;

    while (1) {
        ZeroMemory(&buf, MAX_SIZE);
        if (recv(client_sock, buf, MAX_SIZE, 0) > 0) {

            // 문자열을 스트림에 넣고 공백을 기준으로 분할하여 벡터에 저장
            std::istringstream iss(buf);
            std::vector<std::string> tokens;
            std::string token;

            while (iss >> token) {
                tokens.push_back(token);
            }

            // ( [0] : 요청 결과 (1=로그인 등) / [1] : 보낸 사람 ( 왠만해선 "server") / [2] : 결과값 (ID 찾기 성공 여부) / [3] : 받는 사람 / [4] : 결과(찾은 ID) )
            if (tokens[1] == "server") { // 서버로부터 오는 메시지인 
                if (tokens[0] == "2") {
                    result = tokens[2];
                    if (result == "1") {
                        cout << endl << " ============================================ " << endl;
                        cout << " ※ 아이디 찾기 성공!" << endl;
                        string find_User_id = tokens[4];
                        cout << " ※ " << my_name << " 님의 아이디는 : " << find_User_id << " 입니다." << endl;
                        cout << " ※ 3초 후 메인 화면으로 돌아갑니다." << endl;
                        findID_flag = true;
                        Sleep(3000);
                        break;
                        // 여기에서 결과(result)를 사용하거나 처리
                    }
                    else {
                        findID_flag = false; // ID 찾기 성공 여부
                        cout << endl << " ============================================ " << endl;
                        cout << " ※ ID 찾기 실패. 입력한 정보를 다시 확인해주세요." << endl;
                        Sleep(4000);
                        break;
                    }
                }
                else {
                    findID_flag = false; // ID 찾기 성공 여부
                    cout << endl << " ============================================ " << endl;
                    cout << " ※ ID 찾기 실패. 입력한 정보를 다시 확인해주세요." << endl;
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
//서버로 부터 PW찾기 결과값 받기
int findPW_recv() {
    char buf[MAX_SIZE] = { };
    string msg;

    while (1) {
        ZeroMemory(&buf, MAX_SIZE);
        if (recv(client_sock, buf, MAX_SIZE, 0) > 0) {

            // 문자열을 스트림에 넣고 공백을 기준으로 분할하여 벡터에 저장
            std::istringstream iss(buf);
            std::vector<std::string> tokens;
            std::string token;

            while (iss >> token) {
                tokens.push_back(token);
            }

            // ( [0] : 요청 결과 (1=로그인 등) / [1] : 보낸 사람 ( 왠만해선 "server") / [2] : 결과값 (ID 찾기 성공 여부) / [3] : 받는 사람 / [4] : 결과(찾은 ID) )
            if (tokens[1] == "server") { // 서버로부터 오는 메시지인 
                if (tokens[0] == "3") {
                    result = tokens[2];
                    if (result == "1") {
                        cout << endl << " ============================================ " << endl;
                        cout << " ※ 비밀번호 찾기 성공!" << endl;
                        string find_User_pw = tokens[4];
                        cout << " ※ " << my_name << " 님의 비밀번호는 : " << find_User_pw << " 입니다." << endl;
                        cout << " ※ 3초 후 메인 화면으로 돌아갑니다." << endl;
                        findPW_flag = true;
                        Sleep(3000);
                        break;
                        // 여기에서 결과(result)를 사용하거나 처리
                    }
                    else {
                        findPW_flag = false; // ID 찾기 성공 여부
                        cout << endl << " ============================================ " << endl;
                        cout << " ※ PW 찾기 실패. 입력한 정보를 다시 확인해주세요." << endl;
                        Sleep(4000);
                        break;
                    }
                }
                else {
                    findPW_flag = false; // ID 찾기 성공 여부
                    cout << endl << " ============================================ " << endl;
                    cout << " ※ PW 찾기 실패. 입력한 정보를 다시 확인해주세요." << endl;
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
//로그인
void login()
{
    while (1)
    {
        system("cls");
        loginscreen();

        // 이걸 먼저해야 다시 로그인시도 못함. 
        if (login_flag == true) { break; }


        string User_input;
        string User_request = "1";
        cout << " 아이디 입력 >> ";
        cin >> User_input;
        my_nick = User_input;
        cout << " 비밀번호 입력 >> ";
        cin >> User_input;
        my_pw = User_input;


        string msg = User_request + " " + my_nick + " " + my_pw;
        send(client_sock, msg.c_str(), msg.length(), 0);


        std::thread th2(chat_recv);

        th2.join();
    }
}
//회원가입
void join()
{
    system("cls");
    joinscreen();




    while (!code) {

        if (join_flag == true) { break; }

        // ID입력

        if (join_id_flag == false) {
            while (1)
            {
                cout << "원하시는 ID를 입력하세요. (10자리)>> ";
                cin >> my_id;

                if (my_id.length() > 10) {
                    cout << endl << "========================" << endl;
                    cout << " ※ 입력 가능한 자리수를 넘었습니다. 다시 입력해주세요. ";
                    continue;
                }
                else {
                    break;
                }
            }
            string User_request = "41"; // 회원가입 전 아이디 확인 요청 번호
            string msg_id = User_request + " " + my_id;
            send(client_sock, msg_id.c_str(), msg_id.length(), 0);
        }

        else if (join_id_flag == true) {
            while (1) {
                cout << " 이름을 입력하세요. >> ";
                cin >> my_name;

                if (my_name.length() > 10) {
                    cout << endl << " ============================================ " << endl;
                    cout << " ※ 입력 가능한 자리수를 넘었습니다. 다시 입력해주세요. ";
                    continue;
                }
                else { break; }
            }

            while (1) {
                cout << " 비밀번호를 입력하세요. >> ";
                cin >> my_pw;

                if (my_pw.length() > 10) {
                    cout << endl << " ============================================ " << endl;
                    cout << " ※ 입력 가능한 자리수를 넘었습니다. 다시 입력해주세요. ";
                    continue;
                }
                else { break; }
            }

            while (1) {
                cout << " 전화번호를 입력하세요. >> ";
                cin >> number;

                if (number.length() > 11) {
                    cout << endl << " ============================================ " << endl;
                    cout << " ※ 잘못 입력하셨습니다. 다시 입력해주세요. ";
                    continue;
                }
                else { break; }
            }

            while (1) {
                cout << " 닉네임을 입력하세요. >> ";
                cin >> my_nick;

                if (my_nick.length() > 10) {
                    cout << endl << " ============================================ " << endl;
                    cout << " ※ 입력 가능한 자리수를 넘었습니다. 다시 입력해주세요. ";
                    continue;
                }
                else { break; }
            }


            string first = number.substr(0, 3);
            string second = number.substr(3, 4);
            string third = number.substr(7, 4);
            my_phonenumber = first + "-" + second + "-" + third;
            string User_request = "4"; // 회원가입 요청 번호
            string msg = User_request + " " + my_id + " " + my_name + " " + my_pw + " " + my_phonenumber + " " + my_nick; // 친구 목록은 제외?
            send(client_sock, msg.c_str(), msg.length(), 0);

        }

        std::thread th(join_recv);

        th.join();
    }
}
//서버로 부터 회원가입 결과값 받기
int join_recv()
{
    char buf[MAX_SIZE] = { };
    string msg;


    while (1) {
        ZeroMemory(&buf, MAX_SIZE);
        if (recv(client_sock, buf, MAX_SIZE, 0) > 0) {

            // 문자열을 스트림에 넣고 공백을 기준으로 분할하여 벡터에 저장
            std::istringstream iss(buf);
            std::vector<std::string> tokens;
            std::string token;

            while (iss >> token) {
                tokens.push_back(token);
            }

            // ( [0] : 요청 결과 (1=로그인 등) / [1] : 보낸 사람 ( 왠만해선 "server") / [2] : 결과값 (회원가입 성공 여부) / [3] : 받는 사람 / [4] : 유저 이름 )
            if (tokens[1] == "server") {
                if (tokens[0] == "4") {
                    result = tokens[2];
                    if (result == "1") {
                        cout << endl << " ============================================ " << endl;
                        cout << " ※ 회원 가입 성공!" << endl;
                        //cout << " ※ " << my_name << " 님 환영합니다. " << endl;
                        //cout << " ※ 4초 뒤 메인 화면으로 돌아갑니다. " << endl;
                        join_flag = true; // 회원가입 성공 여부
                        joinsuccessscreen();
                        //Sleep(4000);
                        break;
                    }
                    else if (result == "3") {
                        join_flag = false;
                        cout << endl << " ============================================ " << endl;
                        cout << " ※ 입력한 아이디가 존재하지 않습니다." << endl;
                        cout << " ※ 회원가입을 진행합니다." << endl;
                        cout << " ============================================ " << endl;
                        join_id_flag = true;
                        Sleep(4000);
                        break;
                    }
                    else if (result == "4") {
                        cout << endl << " ============================================ " << endl;
                        cout << " ※ 회원가입 실패! 이미 존재하는 아이디입니다. " << endl;
                        join_flag = false;
                        join_id_flag = false;
                        Sleep(4000);
                        join();
                        break;
                    }
                    else {
                        join_flag = false; // 회원가입 성공 여부
                        cout << endl << " ============================================ " << endl;
                        cout << " ※ 회원가입 실패! 입력한 정보를 다시 확인해주세요. " << endl;
                        Sleep(4000);
                        join();
                        break;
                    }
                }
                else {
                    join_flag = false; // 회원가입 성공 여부
                    cout << endl << " ============================================ " << endl;
                    cout << " ※ 회원가입 실패! 입력한 정보를 다시 확인해주세요. " << endl;
                    Sleep(4000);
                    join();
                    break;
                }
            }


        }
    }
}
//채팅방 입장& 클라이언트에서 입력한 채팅 서버로 보내기
void chatting()
{
    std::thread th2(chat_recv);
    system("cls");
    string User_request = "5";          // 서버에 User_request = "5"을 전송하여 이전 내용을 보기 위해
    send(client_sock, User_request.c_str(), User_request.length(), 0);



    while (1) {
        string User_request = "51";         // 채팅
        string user_msg;
        getline(cin, user_msg); // 사용자 입력을 한 줄로 읽기

        string msg = User_request + " " + user_msg;
        send(client_sock, msg.c_str(), msg.length(), 0);

    }

    th2.join();

}
//서버로 부터 PW 변경 결과값 받기
int edit_recv() {
    string msg;

    while (1) {
        char buf[MAX_SIZE] = { };
        ZeroMemory(&buf, MAX_SIZE);
        if (recv(client_sock, buf, MAX_SIZE, 0) > 0) {

            // 문자열을 스트림에 넣고 공백을 기준으로 분할하여 벡터에 저장
            std::istringstream iss(buf);
            std::vector<std::string> tokens;
            std::string token;

            while (iss >> token) {
                tokens.push_back(token);
            }

            // ( [0] : 요청 결과 (1=로그인 등) / [1] : 보낸 사람 ( 왠만해선 "server") / [2] : 결과값 (ID 찾기 성공 여부) / [3] : 받는 사람 / [4] : 결과(찾은 ID) )
            if (tokens[1] == "server") { // 서버로부터 오는 메시지인 
                if (tokens[0] == "8") {
                    result = tokens[2];
                    if (result == "1") {
                        cout << endl << " ============================================ " << endl;
                        cout << " ※ 입력하신 비밀번호가 일치합니다." << endl;
                        cout << " ※ 3초 후 변경 페이지로 넘어갑니다." << endl;
                        Sleep(3000);
                        system("cls");
                        edit_Menu();

                        cout << " ============================================ " << endl;
                        cout << " ※ 변경하실 비밀번호를 입력하세요. >> ";
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
                        // 여기에서 결과(result)를 사용하거나 처리
                    }

                    else if (result == "2") {
                        cout << endl << " ============================================ " << endl;
                        cout << " ※ 비밀번호를 잘못 입력하셨습니다. " << endl;

                        Sleep(4000);
                        break;
                        /*User_Edit();*/

                    }
                    else if (result == "3") {
                        cout << endl << " ============================================ " << endl;
                        cout << " ※ 비밀번호가 변경되었습니다." << endl;
                        User_Edit_falg = true;
                        tokens.clear();
                        Sleep(4000);
                        break;
                    }
                    else {
                        cout << endl << " ============================================ " << endl;
                        cout << " ※ 변경실패." << endl;
                        Sleep(4000);
                    }
                }
                else {
                    cout << endl << " ============================================ " << endl;
                    cout << " ※ 변경실패." << endl;
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
//PW 변경
void edit_pw()
{
    edit_check = "N";
    while (edit_check == "N") {
        system("cls");
        editpwscreen();

        //edit_check 의 기본값 N
        string User_input;
        string User_request = "8";

        cout << " ============================================ " << endl;
        cout << " " << login_User_nick << " 님의 비밀번호 변경 페이지" << endl;
        cout << " [보안] 현재 비밀번호를 입력하세요. >> ";
        cin >> User_input;
        User_edit_pw = User_input;

        std::thread th2(edit_recv);

        string msg = User_request + " " + User_edit_pw + " " + login_User_id + " " + edit_check;
        send(client_sock, msg.c_str(), msg.length(), 0);



        th2.join();
    }
}

//닉네임은 아직 미완성
string change_nick;

void editnickscreen()
{
    cout << "현재 닉네임 : " << login_User_nick;
    cout << "변경하실 닉네임을 입력해주세요 :";



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

//회원정보 수정
void edit_user()
{
    while (1) {
        system("cls");
        edit_screen();

        if (editchoice == 1) { // pw변경
            edit_pw();
            break;
        }
        else if (editchoice == 2) { // 닉네임 변경
            edit_nick();
            break;
        }
        else if (editchoice == 3) { // 종료
            break;
        }
        else {
            cout << "잘못된 입력입니다. 다시 입력해주세요";
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

            if (choice == 1) { // 로그인
                login();
            }
            else if (choice == 2) { // id찾기
                findID();
            }
            else if (choice == 3) { // pw찾기
                findPW();
            }
            else if (choice == 4) { // 회원가입
                join();
            }
            else { // 종료
                cout << "!!" << endl << endl;
                WSACleanup();
                return 0;
            }
            system("cls");
        }

        else {
            userscreen();

            if (choice == 1) { // 채팅하기
                chatstart = true;
                chatting();

            }
            else if (choice == 2) { // 회원정보 수정
                edit_user();
            }
            else if (choice == 3) { //로그아웃

                login_flag = false;
            }

            else if (choice == 4) { // 종료
                cout << "!!" << endl << endl;
                WSACleanup();
                return 0;
            }
            system("cls");

        }


    }
}


