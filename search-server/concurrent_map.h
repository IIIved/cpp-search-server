#include <map>
#include <mutex>
#include <vector>

using namespace std::string_literals;

template <typename Key, typename Value>
class ConcurrentMap {
public:
    static_assert(std::is_integral_v<Key>,
                  "ConcurrentMap supports only integer keys"s);

    struct Bucket {
        std::mutex mutex_value;
        std::map<Key, Value> container;
    };

    struct Access {
        Access(const Key& key, Bucket& bucket)
            : guard(bucket.mutex_value)
            , ref_to_value(bucket.container[key])
        {}

        std::lock_guard<std::mutex> guard;
        Value& ref_to_value;
    };

    explicit ConcurrentMap(size_t bucket_count)
        : buckets_(bucket_count)
    {}

    Access operator[](const Key& key) {
        uint64_t id = key;
        Bucket& bucket = buckets_[id % buckets_.size()];
        return {key, bucket};
    };

    void Erase(const Key& key) {
        uint64_t id = key;
        Bucket& bucket = buckets_[id % buckets_.size()];
        std::lock_guard guard(bucket.mutex_value);
        bucket.container.erase(key);
    }

    std::map<Key, Value> BuildOrdinaryMap() {
        std::map<Key, Value> result;
        for (auto& [mutex, container] : buckets_) {
            std::lock_guard guard(mutex);
            result.insert(container.begin(), container.end());
        }
        return result;
    };

private:
    std::vector<Bucket> buckets_;
};
