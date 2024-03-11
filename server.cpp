//채팅 서버
#pragma comment(lib, "ws2_32.lib") //명시적인 라이브러리의 링크. 윈속 라이브러리 참조
#include <WinSock2.h>
#include <string>
#include <iostream>
#include <thread>
#include <vector>
#include <mysql/jdbc.h> // C++하고 MYSQL을 연결하기 위해 선언.
#include <sstream> // 요구사항 분리해서 저장하기 위해 필요함.
#include <WS2tcpip.h> // 소켓 전송을 위해 사용
#include <mutex>

#define MAX_SIZE 1024
#define MAX_CLIENT 5

using std::cout;
using std::cin;
using std::endl;
using std::string;

// MY SQL 관련 정보를 저장하는 선언하는 곳 //
const string server = "tcp://127.0.0.1:3306"; // 데이터베이스 주소
const string username = "root"; // 데이터베이스 사용자
const string password = "ek970113"; // 데이터베이스 접속 비밀번호

// MySQL Connector/C++ 초기화
sql::mysql::MySQL_Driver* driver; // 추후 해제하지 않아도 Connector/C++가 자동으로 해제해 줌
sql::Connection* con;
sql::Statement* stmt;
sql::PreparedStatement* pstmt;
sql::ResultSet* res; //결과값을 위해


SOCKET client_sock;

struct SOCKET_INFO { // 연결된 소켓 정보에 대한 틀 생성
    SOCKET sck;
    string user;
    int user_number;
    bool login_status = false;
    int ti;
};

std::vector<SOCKET_INFO> sck_list; // 연결된 클라이언트 소켓들을 저장할 배열 선언.
SOCKET_INFO server_sock; // 서버 소켓에 대한 정보를 저장할 변수 선언.

std::vector<std::string> tokens; // 소켓에 대한 정보를 담기 위한 토큰 배열
std::string token; //토큰 배열
std::mutex mtx;

int client_count = 0; // 현재 접속해 있는 클라이언트를 count 할 변수 선언.
string test_count;
string chatroom_num;

bool login_result = false;
bool join_result = false;
bool join_check = false;
bool user_check = false;
void server_init(); // socket 초기화 함수. socket(), bind(), listen() 함수 실행됨. 자세한 내용은 함수 구현부에서 확인.
void add_client(); // 소켓에 연결을 시도하는 client를 추가(accept)하는 함수. client accept() 함수 실행됨. 자세한 내용은 함수 구현부에서 확인.
void send_msg(const char* msg); // send() 함수 실행됨. 자세한 내용은 함수 구현부에서 확인.
void recv_msg(int idx); // recv() 함수 실행됨. 자세한 내용은 함수 구현부에서 확인.
//void dm_send_msg(const string& sender, const char* msg, const string& recipientUser);
void dm_send_result(int server_request, const string& sender, int variable, const string& recipientUser, const string& username, const string& userid);
void dm_send_findResult(int server_request, const string& sender, int variable, const string& recipientUser, string findValue);
void dm_send_resultEdit(int server_request, const string& sender, int variable, const string& recipientUser);
//int chat_recv();
void db_total_msg();

SOCKET getSocket(string user);
//int removeSocket(string user);
// 디비 구분
void del_client(int idx); // 소켓에 연결되어 있는 client를 제거하는 함수. closesocket() 실행됨. 자세한 내용은 함수 구현부에서 확인.
// MYSQL DB 관련 쿼리문 함수 구현부 //
// 나중에 총 인원수 세기 등 함수 추가할 필요성 있음!! init 과 delete로 디비 정리 필수. //
void db_init(); // db 베이스 선택 및 한글 세팅 관련 부분 구현
//void db_messageSend(); // 메시지 전송 저장
void db_join(); //회원가입
void db_join_check(); // 회원가입 전 아이디 체크
void db_UserEdit(); // 회원 정보 수정할 때 비밀번호 확인문
void db_UserEdit_update(); // 회원 정보 수정 업데이트문
void db_login();
void db_findID(); // (7) 유저 정보 찾기
void db_chat(int idx, string msg);

void send_pastchat(const char* msg, const string& recipientUser);

void db_init() {
    try {
        driver = sql::mysql::get_mysql_driver_instance();
        con = driver->connect(server, username, password);
    }
    catch (sql::SQLException& e) {
        cout << "Could not connect to server. Error message: " << e.what() << endl;
        exit(1);
    }
    // 데이터베이스 선택
    con->setSchema("chatproject");
    // db 한글 저장을 위한 셋팅
    stmt = con->createStatement();
    stmt->execute("set names euckr");
    if (stmt) { delete stmt; stmt = nullptr; }
}

void server_init() {
    server_sock.sck = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    // Internet의 Stream 방식으로 소켓 생성
   // SOCKET_INFO의 소켓 객체에 socket 함수 반환값(디스크립터 저장)
   // 인터넷 주소체계, 연결지향, TCP 프로토콜 쓸 것.
    SOCKADDR_IN server_addr = {}; // 소켓 주소 설정 변수
    // 인터넷 소켓 주소체계 server_addr
    server_addr.sin_family = AF_INET; // 소켓은 Internet 타입
    server_addr.sin_port = htons(7777); // 서버 포트 설정
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);  // 서버이기 때문에 local 설정한다.
    //Any인 경우는 호스트를 127.0.0.1로 잡아도 되고 localhost로 잡아도 되고 양쪽 다 허용하게 할 수 있따. 그것이 INADDR_ANY이다.
    //ip 주소를 저장할 땐 server_addr.sin_addr.s_addr -- 정해진 모양?
    bind(server_sock.sck, (sockaddr*)&server_addr, sizeof(server_addr)); // 설정된 소켓 정보를 소켓에 바인딩한다.
    listen(server_sock.sck, SOMAXCONN); // 소켓을 대기 상태로 기다린다.
    server_sock.user = "server";
    cout << "Server On" << endl;
}

//로그인
void db_login() {

    pstmt = con->prepareStatement("SELECT user_id, name, pw FROM users WHERE user_id = ?");
    pstmt->setString(1, tokens[1]);
    res = pstmt->executeQuery();

    // 결과가 있다면
    if (res->next()) {
        string db_id = res->getString("user_id"); // 데이터베이스의 id 저장
        string db_pw = res->getString("pw"); // 데이터베이스의 비밀번호 저장
        string db_name = res->getString("name"); // 데이터베이스의 이름 저장

        // 데이터베이스에 저장된 데이터와 입력받은 데이터가 동일하다면
        if (db_id == tokens[1] && db_pw == tokens[2]) {
            login_result = true;
            int result = 1;
            int server_request = 1;
            int str_test_count = stoi(test_count);
            sck_list[str_test_count].user = db_name;
            dm_send_result(server_request, "server", result, test_count, db_name, db_id);
        }
        else if (db_id != tokens[1] || db_pw != tokens[2]) {
            int result = 0;
            int server_request = 1;
            dm_send_result(server_request, "server", result, test_count, "임시유저", "temp");
        }
    }
    else {
        int result = 2;
        int server_request = 1;
        dm_send_result(server_request, "server", result, test_count, "임시유저", "temp");
    }
    return;
}
//회원가입
void db_join() {

    pstmt = con->prepareStatement("SELECT user_id, name, pw, phonenumber, nickname FROM users WHERE user_id = ?");
    pstmt->setString(1, tokens[1]);
    res = pstmt->executeQuery();

    if (res->next()) {
        join_result = false;
        int result = 3;
        int server_request = 4;
        int str_test_count = stoi(test_count);
        dm_send_result(server_request, "server", result, test_count, tokens[2], "temp");
    }
    else {

        stmt = con->createStatement();
        pstmt = con->prepareStatement("INSERT INTO users (user_id, name, pw, phonenumber, nickname) values(?,?,?,?,?)"); // INSERT

        pstmt->setString(1, tokens[1]); //아이디
        pstmt->setString(2, tokens[2]); // 이름
        pstmt->setString(3, tokens[3]); // 비밀번호
        pstmt->setString(4, tokens[4]); // 전화번호
        pstmt->setString(5, tokens[5]); // 닉네임

        pstmt->execute(); // 이거 있어야지 디비에 저장됨.

        join_result = true;
        int result = 1;
        int server_request = 4;
        int str_test_count = stoi(test_count);
        sck_list[str_test_count].user = tokens[1];
        dm_send_result(server_request, "server", result, test_count, tokens[2], "temp");
        cout << "신규 계정 생성이 완료되었습니다." << endl;
    }
}
//db아이디 체크 
void db_join_check() {

    pstmt = con->prepareStatement("SELECT user_id FROM users WHERE user_id = ?");
    pstmt->setString(1, tokens[1]);
    res = pstmt->executeQuery();

    if (res->next()) {
        join_check = false;
        user_check = true;
        int result = 4; // 아이디 체크 실패
        int server_request = 4;
        int str_test_count = stoi(test_count);
        dm_send_result(server_request, "server", result, test_count, tokens[1], "temp");
    }

    else {
        join_check = true;
        user_check = false;
        int result = 3; // 아이디 체크 성공 결과값
        int server_request = 4;
        int str_test_count = stoi(test_count);
        dm_send_result(server_request, "server", result, test_count, tokens[1], "temp");
    }
}
//db PW찾기
void db_findPW() {
    pstmt = con->prepareStatement("SELECT user_id, name, pw, phonenumber FROM users WHERE user_id = ?");
    pstmt->setString(1, tokens[1]);
    res = pstmt->executeQuery();

    // 결과가 있다면
    if (res->next()) {
        string db_id = res->getString("user_id");
        string db_name = res->getString("name");
        string db_pw = res->getString("pw");
        string db_phonenumber = res->getString("phonenumber");

        // 데이터베이스에 저장된 데이터와 입력받은 데이터가 동일하다면
        if (db_id == tokens[1] && db_name == tokens[2] && db_phonenumber == tokens[3]) {
            int result = 1; // 성공 시 결과값
            int server_request = 3; // PW 찾기 번호
            int str_test_count = stoi(test_count);
            dm_send_findResult(server_request, "server", result, test_count, db_pw); // findValue = user_id
        }
        else {
            int result = 2; // 실패 시 결과값
            int server_request = 3; // PW 찾기 번호
            string fail = "실패";
            int str_test_count = stoi(test_count);
            dm_send_findResult(server_request, "server", result, test_count, fail); // findValue = fail
        }
    }
    else {
        int result = 2; // 실패 시 결과값
        int server_request = 2; // ID 찾기 번호
        string fail = "실패";
        int str_test_count = stoi(test_count);
        dm_send_findResult(server_request, "server", result, test_count, fail); // findValue = fail
    }
}
//db ID찾기
void db_findID() {
    pstmt = con->prepareStatement("SELECT user_id, name, phonenumber FROM users WHERE phonenumber = ?");
    pstmt->setString(1, tokens[2]);
    res = pstmt->executeQuery();

    // 결과가 있다면
    if (res->next()) {
        string db_id = res->getString("user_id");
        string db_name = res->getString("name");
        string db_phonenumber = res->getString("phonenumber");

        // 데이터베이스에 저장된 데이터와 입력받은 데이터가 동일하다면
        if (db_name == tokens[1] && db_phonenumber == tokens[2]) {
            int result = 1; // 성공 시 결과값
            int server_request = 2; // ID 찾기 번호
            int str_test_count = stoi(test_count);
            dm_send_findResult(server_request, "server", result, test_count, db_id); // findValue = user_id
        }
        else {
            int result = 2; // 실패 시 결과값
            int server_request = 2; // ID 찾기 번호
            string fail = "실패";
            int str_test_count = stoi(test_count);
            dm_send_findResult(server_request, "server", result, test_count, fail); // findValue = fail
        }
    }
    else {
        int result = 2; // 실패 시 결과값
        int server_request = 2; // ID 찾기 번호
        string fail = "실패";
        int str_test_count = stoi(test_count);
        dm_send_findResult(server_request, "server", result, test_count, fail); // findValue = fail
    }
}
//db에서 PW찾아서 변경
void db_UserEdit() {
    // 데이터베이스에서 현재 비밀번호를 가져오는 쿼리
    string selectQuery = "SELECT pw FROM users WHERE user_id = ?";
    pstmt = con->prepareStatement(selectQuery);
    pstmt->setString(1, tokens[2]);
    res = pstmt->executeQuery();

    if (res->next()) {
        string database_password = res->getString("pw");

        // 사용자가 입력한 비밀번호와 데이터베이스의 비밀번호 비교
        if (tokens[1] == database_password) {
            // 입력한 비밀번호와 데이터베이스 비밀번호가 일치하면 업데이트 수행
            cout << "확인 되었습니다." << endl;
            int result = 1;
            int str_test_count = stoi(test_count);
            dm_send_resultEdit(8, "server", result, test_count);
        }
        else {
            cout << "입력한 비밀번호가 일치하지 않습니다." << endl;
            int result = 2;
            int str_test_count = stoi(test_count);
            dm_send_resultEdit(8, "server", result, test_count);
        }
    }
    else {
        cout << "에러 : 사용자의 로그인 정보를 확인할 수 없습니다. (사용자 아이디값 변질)" << endl;
    }
}
//db PW 변경
void db_UserEdit_update() {

    string updateQuery = "UPDATE users SET pw = ? WHERE user_id = ?";
    pstmt = con->prepareStatement(updateQuery);
    pstmt->setString(1, tokens[1]); //토큰즈1이 바꿀비밀번호
    pstmt->setString(2, tokens[2]); //토큰즈2가 유저아이디
    pstmt->executeUpdate();

    cout << tokens[2] << " 의 비밀번호가 변경 되었습니다." << endl;
    int result = 3;
    int str_test_count = stoi(test_count);
    dm_send_resultEdit(8, "server", result, test_count);

}
//닉네임 미완성
void db_UserEditnick_update() {
    string updateQuery = "UPDATE users SET nickname = ? WHERE user_id = ?";
    pstmt = con->prepareStatement(updateQuery);
    pstmt->setString(1, tokens[1]); //토큰즈1이 바꿀비밀번호
    pstmt->setString(2, tokens[2]); //토큰즈2가 유저아이디
    pstmt->executeUpdate();

    cout << "닉네임이 변경되었습니다." << endl;
    int result = 3;
    int str_test_count = stoi(test_count);
    dm_send_resultEdit(9, "server", result, test_count);
}
//db에 채팅내용 넣기
void db_chat(int idx, string msg) {
    db_init();
    pstmt = con->prepareStatement("INSERT INTO chatting(name,contents) VALUES(?,?)");
    pstmt->setString(1, sck_list[idx].user + "  : ");
    pstmt->setString(2, msg);
    pstmt->execute();


}
//db에서 이전 채팅내용 불러오기
void db_total_msg() {

    db_init();
    pstmt = con->prepareStatement("SELECT * FROM chatting");

    res = pstmt->executeQuery();

    while (res->next()) {
        string text;
        int str_test_count = stoi(test_count);
        /*std::cout << "ID: " << res->getString("name") << ", User: " << res->getString("name") << ", Message: " << res->getString("contents") << std::endl;*/
        text = res->getString("name") + res->getString("contents") + "\n";
        //cout << text << endl;         //서버에 이전 메세지가 뜨게 하고 싶을때


        send_pastchat(text.c_str(), test_count);

    }


}

// 특정 사용자에게 메세지
void dm_send_result(int server_request, const string& sender, int variable, const string& recipientUser, const string& username, const string& userid) {
    string vari = std::to_string(variable);
    string serv_request = std::to_string(server_request);
    string result = serv_request + " " + sender + " " + vari + " " + recipientUser + " " + username + " " + userid;
    for (int i = 0; i < client_count; i++) {

        if (std::to_string(sck_list[i].user_number) == recipientUser) {
            send(sck_list[i].sck, result.c_str(), result.length(), 0);
            return; // 특정 사용자에게 메시지를 보내면 함수 종료
        }
    }
    // 사용자를 찾지 못한 경우, 에러 메시지 출력 또는 다른 처리를 추가할 수 있습니다.
}

void dm_send_resultEdit(int server_request, const string& sender, int variable, const string& recipientUser) {
    string vari = std::to_string(variable);
    string serv_request = std::to_string(server_request);
    string result = serv_request + " " + sender + " " + vari + " " + recipientUser;
    for (int i = 0; i < client_count; i++) {

        if (std::to_string(sck_list[i].user_number) == recipientUser) {
            send(sck_list[i].sck, result.c_str(), result.length(), 0);
            return; // 특정 사용자에게 메시지를 보내면 함수 종료
        }
    }
    // 사용자를 찾지 못한 경우, 에러 메시지 출력 또는 다른 처리를 추가할 수 있습니다.
}
//이전 채팅내용 전달(입장하는 클라이언트에게만)
void send_pastchat(const char* msg, const string& recipientUser) {
    string result = msg;

    for (int i = 0; i < client_count; i++) {

        if (std::to_string(sck_list[i].user_number) == recipientUser) {
            send(sck_list[i].sck, result.c_str(), result.length(), 0);
            return; // 특정 사용자에게 메시지를 보내면 함수 종료
        }
    }
}
//ID,PW찾기 결과 전송
void dm_send_findResult(int server_request, const string& sender, int variable, const string& recipientUser, string findValue) {
    string vari = std::to_string(variable);
    string serv_request = std::to_string(server_request);
    string result = serv_request + " " + sender + " " + vari + " " + recipientUser + " " + findValue;
    for (int i = 0; i < client_count; i++) {

        if (std::to_string(sck_list[i].user_number) == recipientUser) {
            send(sck_list[i].sck, result.c_str(), result.length(), 0);
            return; // 특정 사용자에게 메시지를 보내면 함수 종료
        }
    }

    // 사용자를 찾지 못한 경우, 에러 메시지 출력 또는 다른 처리를 추가할 수 있습니다.
}

int main() {
    WSADATA wsa;
    system("title 서버");
    // Winsock를 초기화하는 함수. MAKEWORD(2, 2)는 Winsock의 2.2 버전을 사용하겠다는 의미.
    // 실행에 성공하면 0을, 실패하면 그 이외의 값을 반환.
    // 0을 반환했다는 것은 Winsock을 사용할 준비가 되었다는 의미.
    int code = WSAStartup(MAKEWORD(2, 2), &wsa);
    if (!code) {
        server_init();
        std::thread th1[MAX_CLIENT];
        for (int i = 0; i < MAX_CLIENT; i++) {
            // 인원 수 만큼 thread 생성해서 각각의 클라이언트가 동시에 소통할 수 있도록 함.
            th1[i] = std::thread(add_client);
        }
        //recv_msg(sck_list[client_count].user_number);
        //std::thread th1(add_client); // 이렇게 하면 하나의 client만 받아짐...
        while (1) { // 무한 반복문을 사용하여 서버가 계속해서 채팅 보낼 수 있는 상태를 만들어 줌. 반복문을 사용하지 않으면 한 번만 보낼 수 있음.            
            string text, msg = "";
            std::getline(cin, text);
            const char* buf = text.c_str();
            //msg = buf;
            msg = server_sock.user + " : " + buf;
            send_msg(msg.c_str());
        }
        for (int i = 0; i < MAX_CLIENT; i++) {
            th1[i].join();
            //join : 해당하는 thread 들이 실행을 종료하면 리턴하는 함수.
            //join 함수가 없으면 main 함수가 먼저 종료되어서 thread가 소멸하게 됨.
            //thread 작업이 끝날 때까지 main 함수가 끝나지 않도록 해줌.
        }
        closesocket(server_sock.sck);
    }
    else {
        cout << "프로그램 종료. (Error code : " << code << ")";
    }

    WSACleanup();
    return 0;

}

void add_client() {
    SOCKADDR_IN addr = {};
    int addrsize = sizeof(addr);
    char buf[MAX_SIZE] = { };
    ZeroMemory(&addr, addrsize); // addr의 메모리 영역을 0으로 초기화

    SOCKET_INFO new_client = {};
    new_client.sck = accept(server_sock.sck, (sockaddr*)&addr, &addrsize);
    recv(new_client.sck, buf, MAX_SIZE, 0);
    // Winsock2의 recv 함수. client가 보낸 닉네임을 받음.
    //new_client.user = string(buf);
    cout << "buf = > " << buf << endl;

    sck_list.push_back(new_client); // client 정보를 답는 sck_list 배열에 새로운 client 추가        
    std::thread th(recv_msg, client_count);

    sck_list[client_count].login_status = false;
    sck_list[client_count].user_number = client_count;
    cout << "========================" << endl;
    cout << "새로운 유저가 접근했습니다." << endl;
    cout << "sck_list[client_count].login_status = " << sck_list[client_count].login_status << endl;
    cout << "sck_list[client_count].user_number = " << sck_list[client_count].user_number << endl;
    cout << "========================" << endl;
    client_count++; // client 수 증가 
    th.join();
}

void send_msg(const char* msg) {
    for (int i = 0; i < client_count; i++) { // 접속해 있는 모든 client에게 메시지 전송
        send(sck_list[i].sck, msg, MAX_SIZE, 0);
    }
}

void recv_msg(int idx) {
    //char buf[MAX_SIZE] = { };
    string msg = "";


    //cout << sck_list[idx].user << endl;
    while (1) {
        char buf[MAX_SIZE] = { };
        ZeroMemory(&buf, MAX_SIZE);
        if (recv(sck_list[idx].sck, buf, MAX_SIZE, 0) > 0) { // 오류가 발생하지 않으면 recv는 수신된 바이트 수를 반환. 0보다 크다는 것은 메시지가 왔다는 것.
            cout << "========================" << endl;
            //mtx.lock();
            msg = buf; //sck_list[idx].user
            //send_msg(msg.c_str());
            std::istringstream iss(buf);
            tokens.clear(); // 이전 토큰을 지우고 새로 시작안하면 값 변질되서 제대로 인식 못함 ㅠㅠㅠㅠㅠ
            test_count = std::to_string(sck_list[idx].user_number);
            while (iss >> token) {
                tokens.push_back(token);
            }
            // 토큰0 기준으로 1:로그인 / 2:id찾기 / 3:pw찾기 / 4:회원가입 / 5:대화 / 6:기존채팅 / 7:친추 / 8:비번수정
            // tokens[0] == 1 이면 로그인 요청
            if (tokens[0] == "1") {
                cout << tokens[1] << "을 아이디 값으로 로그인 요청이 들어왔습니다." << endl;
                db_init();
                db_login();
            };
            // tokens[0] == 2 이면 아이디 찾기 요청
            if (tokens[0] == "2") {
                cout << tokens[1] << " 회원이 아이디 찾기 기능을 요청했습니다." << endl;
                db_init();
                db_findID();
            };
            // tokens[0] == 3 이면 비밀번호 찾기 요청
            if (tokens[0] == "3") {
                cout << tokens[1] << " 회원이 비밀번호 찾기 기능을 요청했습니다." << endl;
                db_init();
                db_findPW();
            };
            // tokens[0] == 4 이면 회원가입 요청
            if (tokens[0] == "4") {
                cout << " 회원가입 요청이 들어왔습니다." << endl;
                db_init();
                db_join();
            };
            if (tokens[0] == "41") {
                cout << " 아이디 확인 요청이 들어왔습니다." << endl;
                db_init();
                db_join_check();
            };
            //tokens[0] == 51 이면 보낸 메시지 저장 요청
            if (tokens[0] == "5") {  // 이전 메세지가 나오도록
                db_total_msg();
            }

            if (tokens[0] == "51") {

                if (tokens[1].empty() == false) {
                    size_t pos = msg.find("51");
                    msg.erase(pos, 3);

                    db_chat(idx, msg);

                    msg = sck_list[idx].user + " : " + msg;
                    cout << msg << endl;

                    send_msg(msg.c_str());

                }


            }


            // tokens[0] == 8 이면 비밀번호 수정 요청
            if (tokens[0] == "8") {
                if (tokens[3] == "N") { //tokens[0] 이 8 이면서 tokens[3] 의 값은 Y와 N으로 비밀번호 확인 결과를 결정합니다.
                    cout << tokens[1] << " [비밀번호 확인 요청] 토큰[1]을 비밀번호값으로 바탕으로 비밀번호 변경 요청이 들어왔습니다." << endl;
                    test_count = std::to_string(sck_list[idx].user_number);
                    int result = 0;
                    db_init();
                    db_UserEdit(); // 데이터베이스 쿼리 실행
                    tokens.clear(); // 이전 토큰을 지우고 새로 시작안하면 값 변질되서 제대로 인식 못함 ㅠㅠㅠㅠㅠ
                }
                if (tokens[3] == "Y") {
                    cout << tokens[1] << " [비밀번호 확인 완료] 토큰[1]을 비밀번호값으로 바탕으로 비밀번호 변경 요청이 들어왔습니다." << endl;
                    test_count = std::to_string(sck_list[idx].user_number);
                    int result = 0;
                    db_init();
                    db_UserEdit_update();
                    tokens.clear(); // 이전 토큰을 지우고 새로 시작안하면 값 변질되서 제대로 인식 못함 ㅠㅠㅠㅠㅠ
                }
            }



        }

        else { //그렇지 않을 경우 퇴장에 대한 신호로 생각하여 퇴장 메시지 전송
            msg = "[공지] " + sck_list[idx].user + " 님이 퇴장했습니다.";
            cout << msg << endl;
            send_msg(msg.c_str());
            del_client(idx); // 클라이언트 삭제
            return;
        }
    }
}

void del_client(int idx) {
    closesocket(sck_list[idx].sck);
    client_count--;
}












