#pragma once

#include <cstdint>

#include <map>
#include <optional>
#include <vector>

#include <boost/uuid/uuid.hpp>

#include "asio_with_aliases.h"
#include "forward_call.h"

/*
* Based on https://raft.github.io/raft.pdf
*/

namespace traft {

using NodeId = boost::uuids::uuid;

struct Command {
    // TODO
};

struct LogEntry {
    int32_t term;
    Command command;
};

namespace user {

struct ReadRequest {
    // TODO
};

struct ReadResponse {
    // TODO
};

struct AppendResult {
    // TODO
};

} // namespace user

class Node {
public:
    asio::awaitable<user::AppendResult> userAppendRequest(Command /*command*/) {
        // co_await persistentWrite(command)
        // co_await join(appendRpc)
        co_return user::AppendResult{};
    }
    // TODO: this should be moved to state machine
    // asio::awaitable<user::ReadResponse> userReadRequest(user::ReadRequest request);

    // awaitable<json> appendEntriesRpc()
    // awaitable<json> requestVoteRpc();
private:
    // Persistent state (so it should be saved to disk with flush)
    // int32_t current_term_ = 0;
    std::optional<NodeId> voted_for_ = {};
    std::vector<LogEntry> log_;

    // Volatile state on all nodes
    // int32_t commit_index_ = 0;
    // int32_t last_applied_ = 0;

    // Volatile state on leader
    std::map<NodeId, int32_t> next_index_;
    std::map<NodeId, int32_t> match_index_;
};

} // namespace traft