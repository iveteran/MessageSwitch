#include "sc_command_handler.h"
#include <cassert>
#include <eventloop/tcp_connection.h>
#include "command_messages.h"
#include "switch_client.h"

using namespace evt_loop;


void SCCommandHandler::Echo(const char* content)
{
    size_t sent_bytes = SendCommandMessage(ECommand::ECHO, content);

    if (sent_bytes > 0) {
        printf("Sent ECHO message, content: %s\n", content);
    }
}

void SCCommandHandler::Register(uint32_t ep_id, EEndpointRole ep_role,
        const string& access_code, const string& admin_code)
{
    CommandRegister reg_cmd;
    reg_cmd.id = ep_id > 0 ? ep_id : client_->ID();

    const char* role_str = EndpointRoleToTag(ep_role);
    reg_cmd.role = role_str;
    if (! access_code.empty()) {
        reg_cmd.access_code = access_code;
    }
    if (! admin_code.empty()) {
        reg_cmd.admin_code = admin_code;
    }

    auto content = reg_cmd.encodeToJSON();
    size_t sent_bytes = SendCommandMessage(ECommand::REG, content);

    if (sent_bytes > 0) {
        printf("Sent REG message, content: %s\n", content.c_str());
    }
}

void SCCommandHandler::GetInfo(bool is_details)
{
    CommandInfoReq cmd_info_req;
    cmd_info_req.is_details = is_details;

    //string content(R"({"is_details": true})");
    auto content = cmd_info_req.encodeToJSON();
    size_t sent_bytes = SendCommandMessage(ECommand::INFO, content);

    if (sent_bytes > 0) {
        printf("Sent INFO message, content: %s\n", content.c_str());
    }
}

void SCCommandHandler::ForwardTargets(const vector<uint32_t>& targets)
{
    //string content(R"({"targets": [1, 2]})");
    CommandForward cmd_fwd;
    cmd_fwd.targets = targets;
    auto content = cmd_fwd.encodeToJSON();
    size_t sent_bytes = SendCommandMessage(ECommand::FWD, content);
    if (sent_bytes > 0) {
        printf("Sent FWD message, content: %s\n", content.c_str());
    }
}

void SCCommandHandler::SendData(const string& data)
{
    size_t sent_bytes = SendCommandMessage(ECommand::DATA, data);
    if (sent_bytes > 0) {
        printf("Sent DATA message, content size(%ld):\n", data.size());
        cout << DumpHex(data) << endl;
    }
}

void SCCommandHandler::Setup(const string& admin_code, const string& new_admin_code,
        const string& new_access_code, const string& mode)
{
    CommandSetup cmd_setup;
    cmd_setup.admin_code = admin_code;
    cmd_setup.new_admin_code = new_admin_code;
    cmd_setup.new_access_code = new_access_code;
    cmd_setup.mode = mode;

    auto content = cmd_setup.encodeToJSON();
    size_t sent_bytes = SendCommandMessage(ECommand::SETUP, content);
    if (sent_bytes > 0) {
        printf("Sent SETUP message, content: %s\n", content.c_str());
    }
}

void SCCommandHandler::Kickout(const vector<uint32_t>& targets)
{
    //string content(R"({"targets": [95]})");
    CommandKickout cmd_kickout;
    cmd_kickout.targets = targets;
    auto content = cmd_kickout.encodeToJSON();
    size_t sent_bytes = SendCommandMessage(ECommand::KICKOUT, content);
    if (sent_bytes > 0) {
        printf("Sent KICKOUT message, content: %s\n", content.c_str());
    }
}

void SCCommandHandler::Reload()
{
    size_t sent_bytes = SendCommandMessage(ECommand::RELOAD, "");
    if (sent_bytes > 0) {
        printf("Sent RELOAD message\n");
    }
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

    CommandMessage cmdMsg;
    cmdMsg.cmd = uint8_t(cmd);
    cmdMsg.SetToJSON();
    cmdMsg.payload_len = payload.size();
    reverseToNetworkMessage(&cmdMsg, client_->GetMessageHeaderDescription()->is_payload_len_including_self);

    conn->Send((char*)&cmdMsg, sizeof(cmdMsg));
    size_t sent_bytes = sizeof(cmdMsg);
    if (! payload.empty()) {
        conn->Send(payload);
        sent_bytes += payload.size();
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
