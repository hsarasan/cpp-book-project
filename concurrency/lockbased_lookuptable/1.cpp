#include <mutex>
#include <shared_mutex>
#include <unordered_map>
#include <optional>
#include <vector>

template <
typename Key,
typename Value,
size_t NumBuckets = 19
>
class LookupTable {

public:
    LookupTable() : buckets(NumBuckets) {}
    LookupTable(const LookupTable&) = delete;
    LookupTable& operator=(const LookupTable&) = delete;

    void insert_or_assign(const Key& k, const Value& v) {
        size_t idx = getBucketFor(k);
        std::unique_lock<std::shared_mutex> lock(buckets[idx].bm);
        buckets[idx].umap[k] = v;
    }

    void insert_or_assign(const Key& k, Value&& v) {
        size_t idx = getBucketFor(k);
        std::unique_lock<std::shared_mutex> lock(buckets[idx].bm);
        buckets[idx].umap[k] = std::move(v);
    }

    std::optional<Value> get_value(const Key& k) {
        size_t idx = getBucketFor(k);
        std::shared_lock<std::shared_mutex> lock(buckets[idx].bm);

        auto it = buckets[idx].umap.find(k);
        if (it != buckets[idx].umap.end()) {
            return it->second; 
        }
        return std::nullopt;
    }

    void erase(const Key& k) {
        size_t idx = getBucketFor(k);
        std::unique_lock<std::shared_mutex> lock(buckets[idx].bm);
        buckets[idx].umap.erase(k);
    }

    size_t size() const {
        size_t count = 0;
        for (auto& bucket : buckets) {
            std::shared_lock<std::shared_mutex> lock(bucket.bm);
            count += bucket.umap.size();
        }
        return count;
    }

    void clear() {
        for (auto& bucket : buckets) {
            std::unique_lock<std::shared_mutex> lock(bucket.bm);
            bucket.umap.clear();
        }
    }

private:
    struct Bucket {
        std::unordered_map<Key, Value> umap;
        mutable std::shared_mutex bm;
    };

    std::vector<Bucket> buckets;

    size_t getBucketFor(const Key& k) const {
        return std::hash<Key>{}(k) % NumBuckets;
    }
};
