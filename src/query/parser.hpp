#pragma once

#include <utility>
#include <expected>
#include <vector>
#include <span>
#include <memory>
#include <functional>
#include <map>

#include "../errors/parsing_exception.hpp"
#include "sql_lexer.hpp"

template<typename TResult>
class Parser {
public:
    using Token = SQLLexer::Token;
    using TokenSpan = std::span<Token>;
    using Category = Token::Category;

    using Exception = ParsingException;
    using Subject = TResult;
    using ExpectedResult = std::pair<Subject, TokenSpan>;
    using Result = std::expected<ExpectedResult, Exception>;

    class Node : std::enable_shared_from_this<Node> {
        friend class Parser;
    public:
        using Pointer = std::shared_ptr<Node>;
        using WeakPointer = std::weak_ptr<Node>;
        using Callback = std::function<std::expected<TokenSpan, Exception>(TokenSpan, Subject&)>;

        std::expected<TokenSpan, Exception> execute(const TokenSpan tokens, Subject& subject) const {
            if (tokens.empty()) return {};

            if (callback) return callback(tokens, subject);

            return tokens.subspan(1);
        }

        Pointer getNext(const Category cat) const {
            if (!nextNodes.contains(cat)) return nullptr;

            auto& nextNode = nextNodes.find(cat)->second;
            if (nextNode.expired()) return nullptr;

            return nextNode.lock();
        }

        std::vector<Category> getPossibilities() {
            return nextNodes | std::views::keys | std::ranges::to<std::vector>();
        }

        explicit Node(const Category category, Callback callback = {})
            : cat(category), callback(std::move(callback)) {}
    private:

        void connect(const WeakPointer& node) {
            if (node.expired()) return;

            if (Pointer nodePtr = node.lock(); !nextNodes.try_emplace(nodePtr->cat, node).second) {
                throw std::runtime_error(
                    "Error while connecting nodes in parser. Same path was specified multiple times.");
            }
        }

        Category cat;
        Callback callback{};
        std::map<Category, WeakPointer> nextNodes{};
    };

    explicit Parser()
        : startNode(std::make_shared<Node>(Category::None)),
            endNode(std::make_shared<Node>(Category::None)) {}

    typename Node::WeakPointer createNode(const Category category, const typename Node::Callback& callback = {}) {
        auto node = std::make_shared<Node>(category, callback);
        nodes.emplace_back(node);
        return node;
    }

    typename Node::WeakPointer createLoopedNode(const Category category, const typename Node::Callback& callback, const Category separator) {
        const auto separatorNode = createNode(separator).lock();
        const auto createdNode = createNode(category, callback).lock();

        createdNode->connect(separatorNode);
        separatorNode->connect(createdNode);

        return createdNode;
    }

    template <size_t Count>
    std::array<typename Node::WeakPointer, Count> createNodes(std::array<Category, Count> nodes) {
        std::array<typename Node::WeakPointer, Count> n{};
        for (size_t i = 0; i < nodes.size(); i++) n[i] = createNode(nodes[i]);
        return n;
    }

    static void connect(const typename Node::WeakPointer& source, const typename Node::WeakPointer& destination) {
        source.lock()->connect(destination);
    }

    static void connect(const typename Node::WeakPointer& source, const std::vector<typename Node::WeakPointer>& destinations) {
        auto src = source.lock();
        for (const auto& wp : destinations) src->connect(wp);
    }

    const typename Node::Pointer& getBegin() const { return startNode; }
    const typename Node::Pointer& getEnd() const { return endNode; }
    std::pair<const typename Node::Pointer&, const typename Node::Pointer&> getBoundries() const { return {startNode, endNode}; }

    [[nodiscard]] Result parse(TokenSpan tokens) const {
        TResult result{};

        if (tokens.empty()) return std::unexpected(Exception("There was nothing to parse."));

        auto current = startNode->getNext(tokens.front().category);;
        const auto end = endNode;

        while (!tokens.empty() && current != nullptr) {
            auto res = current->execute(tokens, result);

            if (!res) return std::unexpected(res.error());

            tokens = res.value();

            if (!tokens.empty()) {
                auto next = current->getNext(tokens.front().category);
                if (next == nullptr) next = current->getNext(Category::AnyCategory);
                if (next == nullptr) break;
                current = next;
            }
        }

        const bool wasFinalNode = current->getNext(Category::None) == endNode;

        Category nextCat = !tokens.empty() ? tokens.front().category : Category::None;
        if (!wasFinalNode) return std::unexpected(Exception(nextCat , current->getPossibilities()));

        return ExpectedResult({std::move(result), tokens});
    }
private:
    std::vector<typename Node::Pointer> nodes{};
    typename Node::Pointer startNode, endNode;
};