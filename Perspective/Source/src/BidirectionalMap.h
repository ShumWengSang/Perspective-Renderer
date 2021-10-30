#pragma once
// Immutable
// No remove functions yet
// unordered, different then insertion order
template<typename Key, typename Value>
class BidirectionMap
{
private:
    template<typename T>
    using Container = std::map<size_t, T>;
    using KeyTree = Container<Key>;
    using ValueTree = Container<Value>;

    using IteratorValueType = std::pair<typename KeyTree::const_iterator, typename ValueTree::value_type::second_type>;

    public:
    // Iterators
    class const_iterator
    {
    public:
        using Tree = BidirectionMap<Key, Value>;
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = std::pair<typename KeyTree::const_iterator, typename ValueTree::value_type::second_type>;
        using pointer = value_type*;   // or also value_type*
        using reference = value_type&; // or also value_type&
        using return_type = std::pair<typename std::reference_wrapper<const typename KeyTree::value_type::second_type>, typename std::reference_wrapper<const typename ValueTree::value_type::second_type>>;
        using contructor_type = std::pair<typename KeyTree::const_iterator, std::reference_wrapper<const Tree>>;
        typedef const_iterator self_type;
        explicit const_iterator(const contructor_type& value) : tree(value.second)
        {
            m_data.first = std::move(value.first);
        }
        const self_type& operator++()
        {
            ++m_data.first;
            return *this;
        }
        const self_type& operator--()
        {
            m_data.first--;
            return *this;
        }
        return_type operator*()
        {
            m_data.second = tree.getValue(m_data.first->second);
            return { std::cref(m_data.first->second), std::cref(m_data.second) };
        }


        bool operator==(const self_type& rhs) { return m_data.first == rhs.m_data.first; }
        bool operator!=(const self_type& rhs) { return m_data.first != rhs.m_data.first; }

    private:
        const Tree& tree;
        value_type m_data;
    };

public:
    BidirectionMap() = default;
    BidirectionMap(const BidirectionMap&) = delete;
    void operator=(const BidirectionMap&) = delete;
    BidirectionMap(BidirectionMap&&) = default;
    BidirectionMap& operator=(BidirectionMap&&) = default;

    std::pair<const_iterator, bool> insert(const Key& key, const Value& value)
    {
        size_t hashKey = std::hash<Key>{}(key);
        size_t hashValue = std::hash<Value>{}(value);

        // Check for existance of either in either tree
        auto keyIter = keyTree.find(hashValue);
        auto valIter = valueTree.find(hashKey);
        if (keyIter == keyTree.end() && valIter == valueTree.end())
        {
            auto pair = keyTree.insert({ hashValue, key });
            valueTree.insert({ hashKey, value });

            typename KeyTree::const_iterator citer = pair.first;
            const_iterator thisciter{ { std::move(citer), std::cref(*this) } };
            return { std::move(thisciter), pair.second };
        }
        else
        {
            // It already exists in one tree at least
            // If it exists in key tree
            if (keyIter != keyTree.end())
            {
                typename KeyTree::const_iterator citer = keyIter;
                const_iterator thisciter{ { std::move(citer), std::cref(*this) } };
                return { std::move(thisciter), false };
            }
            else
            {
                // If it exists in the value tree
                // Find the corresponding iterator
                hashValue = std::hash<Value>{}(valIter->second);
                keyIter = keyTree.find(hashValue);
                typename KeyTree::const_iterator citer = keyIter;
                const_iterator thisciter{ { std::move(citer), std::cref(*this) } };
                return { std::move(thisciter), false };
            }
        }
    }

    const Value& getValue(const Key& key) const
    {
        size_t hashKey = std::hash<Key>{}(key);
        auto iter = valueTree.find(hashKey);
        if (iter != valueTree.end())
            return iter->second;
        else
            throw std::runtime_error("Key not in bi map");
    }

    const Key& getKey(const Value& value) const
    {
        size_t hashValue = std::hash<Value>{}(value);
        auto iter = keyTree.find(hashValue);
        if (iter != keyTree.end())
            return iter->second;
        else
            throw std::runtime_error("Value not in bi map");
    }

    bool hasKey(const Key& key) const
    {
        size_t hashKey = std::hash<Key>{}(key);
        return valueTree.find(hashKey) != valueTree.end();
    }
    bool hasValue(const Value& value) const
    {
        size_t valueTree = std::hash<Value>{}(value);
        return keyTree.find(valueTree) != keyTree.end();
    }

    size_t size() const
    {
        return keyTree.size();
    }

    void clear()
    {
        keyTree.clear();
        valueTree.clear();
    }

public:
    // Iterators
    const_iterator begin() const { return const_iterator({ keyTree.cbegin(), std::cref(*this) }); }
    const_iterator end() const { return const_iterator({ keyTree.cend(), std::cref(*this) }); }

private:
    KeyTree keyTree;
    ValueTree valueTree;
};