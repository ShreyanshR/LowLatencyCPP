int main() {
    std::cout << "=== LRU Cache Demo ===" << std::endl << std::endl;

    LRUCache<std::string, int> cache(3);

    // Test 1: Basic put and get
    std::cout << "1. Adding items to cache (capacity = 3)" << std::endl;
    cache.put("a", 1);
    std::cout << "   put('a', 1)" << std::endl;
    cache.put("b", 2);
    std::cout << "   put('b', 2)" << std::endl;
    cache.put("c", 3);
    std::cout << "   put('c', 3)" << std::endl;
    std::cout << "   Cache state: {a=1, b=2, c=3}" << std::endl;

    // Test 2: Get operation updates recency
    std::cout << std::endl << "2. Accessing 'a' makes it most recently used" << std::endl;
    auto valueA = cache.get("a");
    std::cout << "   get('a') = " << (valueA.has_value() ? std::to_string(valueA.value()) : "nullopt") << std::endl;
    std::cout << "   Order now: b (LRU) -> c -> a (MRU)" << std::endl;

    // Test 3: Eviction on capacity overflow
    std::cout << std::endl << "3. Adding 'd' should evict 'b' (the LRU item)" << std::endl;
    cache.put("d", 4);
    std::cout << "   put('d', 4)" << std::endl;

    auto valueB = cache.get("b");
    std::cout << "   get('b') = " << (valueB.has_value() ? std::to_string(valueB.value()) : "nullopt")
              << " (nullopt means evicted)" << std::endl;

    // Test 4: Verify other items still exist
    std::cout << std::endl << "4. Verifying other items still accessible" << std::endl;
    auto valC = cache.get("c");
    auto valA = cache.get("a");
    auto valD = cache.get("d");
    std::cout << "   get('c') = " << (valC.has_value() ? std::to_string(valC.value()) : "nullopt") << std::endl;
    std::cout << "   get('a') = " << (valA.has_value() ? std::to_string(valA.value()) : "nullopt") << std::endl;
    std::cout << "   get('d') = " << (valD.has_value() ? std::to_string(valD.value()) : "nullopt") << std::endl;

    // Test 5: Update existing key
    std::cout << std::endl << "5. Updating existing key" << std::endl;
    cache.put("c", 30);
    std::cout << "   put('c', 30) - updates value and marks as MRU" << std::endl;
    auto updatedC = cache.get("c");
    std::cout << "   get('c') = " << (updatedC.has_value() ? std::to_string(updatedC.value()) : "nullopt") << std::endl;

    // Test 6: Add another item, should evict 'a' now
    std::cout << std::endl << "6. Adding 'e' should evict 'a' (now the LRU)" << std::endl;
    cache.put("e", 5);
    std::cout << "   put('e', 5)" << std::endl;
    auto evictedA = cache.get("a");
    auto stillD = cache.get("d");
    std::cout << "   get('a') = " << (evictedA.has_value() ? std::to_string(evictedA.value()) : "nullopt")
              << " (nullopt means evicted)" << std::endl;
    std::cout << "   get('d') = " << (stillD.has_value() ? std::to_string(stillD.value()) : "nullopt") << std::endl;

    std::cout << std::endl << "=== Demo Complete ===" << std::endl;

    return 0;
}//
// Created by Shreyansh Rathore on 30/05/2026.
//
