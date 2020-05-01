#pragma once

#include <cstdint>

#include <map>
#include <optional>
#include <vector>

#include <boost/uuid/uuid.hpp>

/*
* Fully based on https://raft.github.io/raft.pdf
*/

namespace traft {

using NodeId = boost::uuids::uuid;

struct LogEntry {
    int32_t term;
    // Command command;
};

class Node {
public:
    // awaitable<json> userRequest()

    // awaitable<json> appendEntriesRpc()
    // awaitable<json> requestVoteRpc();
private:
    // Persistent state (so it should be saved to disk with flush)
    int32_t current_term_ = 0;
    std::optional<NodeId> voted_for_ = {};
    std::vector<LogEntry> log_;

    // Volatile state on all nodes
    int32_t commit_index_ = 0;
    int32_t last_applied_ = 0;

    // Volatile state on leader
    std::map<NodeId, int32_t> next_index_;
    std::map<NodeId, int32_t> match_index_;
};

} // namespace traft