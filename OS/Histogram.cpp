#include <vector>
#include <cmath>


struct Bucket {
	int Start, End, Count;
};

using Buckets = std::vector<Bucket>;

class Histogram {
public:
	Histogram(size_t count, size_t size) {
		_count = count;
		_buckets.assign(count+1, 0);
		_size = size;
		_shift = static_cast<size_t>(std::log2(_size));
	}
	Buckets Build(const std::vector<int>& latencies) {
		for (auto& latency: latencies) {
			size_t index = latency >> _shift;
			index = std::min(index, _count);
			_buckets[index] += 1;
		}

		Buckets buckets;
		buckets.assign(_buckets.size(), {0,0,0});

		for (int i = 0; i < _buckets.size(); i++) {
			buckets[i].Start = i * _size;
			buckets[i].End = i == _buckets.size() -1 ? INT_MAX : (i+1) * _size;
			buckets[i].Count = _buckets[i];
		}
		return buckets;
	}
private:
	size_t _count;
	size_t _size;
	std::vector<int> _buckets;
	size_t _shift; //compute the logs shfit to find the bucket

};