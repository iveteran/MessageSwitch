#include "sc_command_handler.h"
#include <cassert>
#include <eventloop/tcp_connection.h>
#include "command_messages.h"
#include "switch_client.h"

using namespace evt_loop;

void SCCommandHandler::Register()
{
    CommandRegister reg_cmd;
    reg_cmd.id = client_->ID();
#if 0
    reg_cmd.role = "endpoint";
    reg_cmd.access_code = "Hello World";
#else
    reg_cmd.role = "admin";
    reg_cmd.admin_code = "Foobar2000";
#endif
    //reg_cmd.role = "service";
    //reg_cmd.access_code = "GOE works";

    auto params_data = reg_cmd.encodeToJSON();

    SendCommandMessage(ECommand::REG, params_data);
    printf("Sent REG message, content: %s\n", params_data.c_str());
}

void SCCommandHandler::GetInfo()
{
    //const char* content = R"({"is_details": true})";
    CommandInfoReq cmd_info_req;
    cmd_info_req.is_details = true;
    auto content = cmd_info_req.encodeToJSON();
    SendCommandMessage(ECommand::INFO, content);
    printf("Sent INFO message, content: %s\n", content.c_str());
}

void SCCommandHandler::SendData()
{
    char content[64] = {0};
    snprintf(content, sizeof(content), "my data #%d", client_->ID());
    SendCommandMessage(ECommand::DATA, content);
    printf("Sent DATA message, content: %s\n", content);
}

void SCCommandHandler::ForwardData()
{
    //const char* content = R"({"targets": [1, 2]})";
    CommandForward cmd_fwd;
    cmd_fwd.targets.push_back(1);
    cmd_fwd.targets.push_back(2);
    auto content = cmd_fwd.encodeToJSON();
    SendCommandMessage(ECommand::FWD, content);
    printf("Sent FWD message, content: %s\n", content.c_str());
}

void SCCommandHandler::Setup()
{
    //const char* content = R"({"new_admin_code": "my_admin_code", "admin_code": "Foobar2000"})";
    //const char* content = R"({"new_access_code": "my_access_code"})";
    //const char* content = R"({"mode": "proxy"})";
    CommandSetup cmd_setup;
    //cmd_setup.new_admin_code = "my_admin_code";
    //cmd_setup.admin_code = "Foobar2000";

    //cmd_setup.new_access_code = "my_access_code";

    cmd_setup.mode = "proxy";
    auto content = cmd_setup.encodeToJSON();
    SendCommandMessage(ECommand::SETUP, content);
    printf("Sent SETUP message, content: %s\n", content.c_str());
}

void SCCommandHandler::Kickout()
{
    //const char* content = R"({"targets": [95]})";
    CommandKickout cmd_kickout;
    cmd_kickout.targets.push_back(95);
    auto content = cmd_kickout.encodeToJSON();
    SendCommandMessage(ECommand::KICKOUT, content);
    printf("Sent KICKOUT message, content: %s\n", content.c_str());
}

void SCCommandHandler::Reload()
{
    SendCommandMessage(ECommand::RELOAD, "");
    printf("Sent RELOAD message\n");
}

size_t SCCommandHandler::SendCommandMessage(ECommand cmd, const string& payload)
{
    return SendCommandMessage(client_->Connection().get(), cmd, payload);
}

size_t SCCommandHandler::SendCommandMessage(TcpConnection* conn, ECommand cmd, const string& payload)
{
    if (! client_->IsConnected()) {
        printf("The connection was disconnected! Do nothing.\n");
        return 0;
    }

    size_t sent_bytes = 0;

    CommandMessage cmdMsg;
    cmdMsg.cmd = uint8_t(cmd);
    cmdMsg.SetToJSON();
    size_t payload_len = payload.size();
    if (client_->GetMessageHeaderDescription()->is_payload_len_including_self) {
        payload_len += sizeof(CommandMessage::payload_len);
    }
    cmdMsg.payload_len = htonl(payload_len);

    conn->Send((char*)&cmdMsg, sizeof(cmdMsg));
    sent_bytes += sizeof(cmdMsg);
    if (! payload.empty()) {
        conn->Send(payload);
        sent_bytes += payload_len;
    }

    return sent_bytes;
}

void SCCommandHandler::HandleCommandResult(TcpConnection* conn, CommandMessage* cmdMsg)
{
    ECommand cmd = (ECommand)cmdMsg->cmd;
    printf("Command result:\n");
    printf("cmd: %s(%d)\n", CommandToTag(cmd), uint8_t(cmd));
    printf("payload_len: %d\n", cmdMsg->payload_len);

    ResultMessage* resultMsg = (ResultMessage*)(cmdMsg->payload);
    int8_t errcode = resultMsg->errcode;
    char* content = resultMsg->data;
    printf("errcode: %d\n", errcode);
    size_t content_len = cmdMsg->payload_len - sizeof(ResultMessage);
    printf("content len: %ld\n", content_len);
    if (errcode == 0) {
        printf("content: %s\n", content);
    } else {
        printf("error message: %s\n", content);
    }

    switch (cmd)
    {
        case ECommand::REG:
            break;
        case ECommand::INFO:
            if (errcode == 0) {
                HandleGetInfoResult(cmdMsg, content);
            }
            break;
        default:
            break;
    }
}

void SCCommandHandler::HandleGetInfoResult(CommandMessage* cmdMsg, const string& data)
{
    CommandInfo cmd_info;
    if (cmdMsg->IsJSON()) {
        cmd_info.decodeFromJSON(data);
    } else if (cmdMsg->IsPB()) {
        cmd_info.decodeFromPB(data);
    } else {
        assert(false && "Unsupported message codec");
    }
    printf("cmd_info.uptime: %ld\n", cmd_info.uptime);
    printf("cmd_info.endpoints.total: %d\n", cmd_info.endpoints.total);
    printf("cmd_info.endpoints.rx_bytes: %d\n", cmd_info.endpoints.rx_bytes);
    printf("cmd_info.admin_clients.total: %d\n", cmd_info.admin_clients.total);
}
