Caching

A cache is a temporary storage layer that stores frequently or recently accessed data so it can be retrieved much faster than fetching it from the original data source (such as a database). By serving data from the cache, applications can reduce latency, improve performance, and decrease the load on the database.

### **External Caching**

A dedicated caching service that is independent of the application servers and the database (e.g., **Redis**, **Memcached**).

* The application server first checks the external cache.
* If the data is found (**cache hit**), it is returned immediately.
* If the data is not found (**cache miss**), the server queries the database, stores the result in the cache, and returns the response.
* Multiple application servers can share the same cached data.
* The cache can be scaled independently of the application servers.
* Commonly used in distributed systems and microservices.
* Supports advanced features such as **TTL (Time-to-Live)**, **eviction policies (LRU/LFU)**, persistence (Redis), and distributed caching.

#### Pros

* Shared cache across multiple application instances.
* Reduces database load.
* Scales independently of the application.
* Cache remains available even if application servers are restarted.

#### Cons

* Additional network hop introduces some latency.
* Requires managing a separate caching infrastructure.
* Cache outages can impact application performance.

---

### **In-Process Caching**

Instead of using an external cache, each application server stores frequently accessed data in its own memory.

* Each application server maintains its own local cache.
* Cached data cannot be shared with other application servers.
* Provides extremely low latency because no network call is required.
* Commonly implemented using in-memory data structures (e.g., **HashMap**, **ConcurrentHashMap**, **Caffeine**, **Guava Cache**).

#### Pros

* Very fast (memory access only).
* No network overhead.
* Simple to implement.
* No external infrastructure is required.

#### Cons

* Cache is local to each application instance.
* Cached data is lost when the application restarts.
* Higher memory usage because each server stores its own copy of the data.
* Cache invalidation is difficult in distributed systems, as updates must be propagated to all application instances.
* Can lead to stale or inconsistent data across servers.


## Cache Architectures

1. **Cache-Aside** (Lazy Loading) — The Most Common

- The application server first checks the cache. If the data is found, it is a **cache hit**, and the server returns the data.
- If the data is not found (**cache miss**), the server queries the database, stores the data in the cache, and then returns it.
- A cache miss adds extra latency because the data must first be fetched from the database.
- This approach is beneficial because the cache stores only the data that is actually requested.

```text
Request
   │
   ▼
Application Server
   │
   ▼
Check Cache
   ├── Hit  ──► Return Data
   └── Miss ──► Query Database ──► Store in Cache ──► Return Data
```

2. **Write-Through (Synchronous)**

- The application server writes data to the cache, which then synchronously writes the data to the database.
- The write is considered successful only after both the cache and the database have been updated.
- Ensures that the cache and the database remain consistent after a successful write.

Cons
- Both the cache write and the database write must succeed for the operation to be considered successful.
- Writes are slower because the application must wait for the database write to complete.
- Can result in unnecessary database writes, even for data that may never be read again.
- If the database write fails, it can lead to inconsistencies or failed write operations.
- Requires an additional library or mechanism to manage synchronous updates to the database.

```text
Application Server
        │
        ▼
      Cache
        │
        ▼
Database (Synchronous Write)
```

3. **Write-Behind (Asynchronous)**

- The application server reads from and writes to the cache.
- The cache asynchronously flushes write operations to the database after a delay or in batches.
- Provides lower write latency because the application does not wait for the database write to complete.

Cons
- Data can be lost if the cache fails before the pending writes are flushed to the database.
- The cache and the database are temporarily inconsistent until the asynchronous flush completes.
- Recovery can be more complex if pending writes are lost.

```text
Application Server
        │
 (Read / Write)
        ▼
      Cache
        │
 (Asynchronous Flush)
        ▼
     Database
```

4. **Read-Through** (CDN)

* The application server requests data from the cache instead of directly querying the database.
* If the data is present (**cache hit**), the cache returns it immediately.
* If the data is not present (**cache miss**), the cache fetches the data from the database, stores it in the cache, and then returns it to the application.
* The application does not need to implement cache-miss handling because the cache manages it automatically.
* Commonly used by CDNs and managed caching systems.

Cons

* Requires a cache that supports read-through functionality.
* A cache miss introduces additional latency because the cache must fetch the data from the database.
* If the cache becomes unavailable, reads may fail unless a fallback mechanism is implemented.

```text
Application Server
        │
        ▼
      Cache
   ├── Hit  ──► Return Data
   └── Miss ──► Fetch from Database ──► Store in Cache ──► Return Data
```

## Cache Eviction Policies

Cache memory is limited, so we cannot store all data indefinitely. A **cache eviction policy** determines which items should be removed when the cache is full or when cached data becomes stale.

### 1. **LRU (Least Recently Used)**

* Evicts the item that has not been accessed for the longest time.
* Assumes that recently accessed data is more likely to be accessed again (**temporal locality**).
* One of the most commonly used eviction policies.
* Provides a good balance between performance and memory utilization.

**Best for:** General-purpose caching where recently accessed data is likely to be reused.

---

### 2. **LFU (Least Frequently Used)**

* Evicts the item with the lowest access frequency.
* Frequently accessed ("hot") data remains in the cache even if it hasn't been accessed recently.
* Better suited for workloads where certain data is consistently popular.
* Slightly more complex than LRU because access counts must be maintained.

**Best for:** Applications with stable access patterns (e.g., popular products, trending content).

---

### 3. **FIFO (First In, First Out)**

* Evicts the oldest item in the cache, regardless of how often or how recently it was accessed.
* Simple and easy to implement.
* Does not consider usage patterns, so frequently accessed items may be removed.

**Best for:** Simple caching systems where implementation simplicity is preferred over optimal hit rate.

---

### 4. **TTL (Time to Live)**

* Each cached item is assigned an expiration time (e.g., 5 minutes).
* Once the TTL expires, the item is removed or refreshed on the next request.
* Helps prevent stale data from remaining in the cache indefinitely.
* TTL is often used together with another eviction policy such as LRU or LFU.

**Best for:** Frequently changing data such as user sessions, API responses, and stock prices.

---

### Other Common Eviction Policies

#### **MRU (Most Recently Used)**

* Evicts the most recently accessed item.
* Useful for workloads where recently used data is unlikely to be accessed again.
* Less common than LRU.

#### **Random Replacement**

* Randomly selects an item for eviction.
* Very simple and has minimal overhead.
* Used in some high-performance systems where tracking access history is expensive.

---

### Choosing an Eviction Policy

| Policy     | Best For                       | Advantage                   | Drawback                                   |
| ---------- | ------------------------------ | --------------------------- | ------------------------------------------ |
| **LRU**    | General-purpose caching        | Simple and effective        | Doesn't consider access frequency          |
| **LFU**    | Frequently accessed (hot) data | Keeps popular data in cache | Higher implementation overhead             |
| **FIFO**   | Simple systems                 | Easy to implement           | Ignores access patterns                    |
| **TTL**    | Time-sensitive data            | Prevents stale data         | Expiration alone doesn't handle full cache |
| **MRU**    | Special workloads              | Useful in niche scenarios   | Rarely beneficial                          |
| **Random** | High-performance systems       | Very low overhead           | Lower cache hit rate                       |

## Caching Issues

### 1. **Cache Stampede (Thundering Herd)**

Occurs when a popular cache entry expires or is missing, causing many concurrent requests to query the database simultaneously.

**Problem**

* Cache miss occurs for a frequently accessed ("hot") key.
* Multiple requests bypass the cache and hit the database at the same time.
* Can overwhelm the database and significantly increase latency.

**Solutions**

* Refresh or preload hot keys before they expire (cache warming).
* Use **mutex/distributed locking** so only one request fetches data from the database while others wait.
* Add **TTL jitter** (randomized expiration times) to prevent many keys from expiring simultaneously.
* Use **stale-while-revalidate**, where stale data is served while the cache is refreshed in the background.
* Enable **request coalescing**, so multiple identical requests share a single database query.

---

### 2. **Cache Consistency**

Occurs when the cache and the database contain different versions of the same data.

**Problem**

* Common in the **Cache-Aside** pattern.
* During a write, the database may be updated while the cache still contains stale data.
* Applications may read outdated values from the cache.

**Solutions**

* After updating the database, **invalidate (delete)** the corresponding cache entry.
* Use **Write-Through** or **Write-Behind** caching when appropriate.
* Assign a **TTL** so stale data is eventually refreshed.
* Use versioning or optimistic locking for highly concurrent systems.
* Publish cache invalidation events (e.g., via Kafka or Redis Pub/Sub) so all application instances invalidate stale entries.

---

### 3. **Cache Penetration**

Occurs when requests repeatedly query data that does **not exist**.

**Problem**

* Every request results in a cache miss.
* The database is queried repeatedly for the same nonexistent key.

**Solutions**

* Cache null/empty results for a short TTL.
* Use a **Bloom Filter** to reject invalid keys before reaching the database.
* Validate request parameters before querying the cache or database.

---

### 4. **Cache Breakdown (Hot Key Expiration)**

Occurs when a single highly popular key expires.

**Problem**

* Thousands of requests attempt to reload the same key simultaneously.
* Can overload the database.

**Solutions**

* Never allow hot keys to expire.
* Refresh hot keys asynchronously before expiration.
* Use distributed locking so only one request reloads the data.

> **Note:** Cache Breakdown is a special case of a Cache Stampede that affects a single hot key.

---

### 5. **Cache Avalanche**

Occurs when a large number of cache entries expire at the same time or the cache service becomes unavailable.

**Problem**

* Massive number of requests fall back to the database.
* Database may become overloaded or unavailable.

**Solutions**

* Add random TTLs (TTL jitter).
* Use cache clustering and replication for high availability.
* Warm the cache after restarts.
* Implement rate limiting and circuit breakers to protect the database.

---

### 6. **Cold Cache (Cold Start)**

Occurs when the cache is empty, such as after deployment or a restart.

**Problem**

* Initial requests experience high latency.
* Increased load on the database until the cache is populated.

**Solutions**

* Preload frequently accessed data (cache warming).
* Gradually route traffic after deployments.
* Keep frequently accessed data persistent if supported by the cache.

---

### Summary

| Issue                 | Cause                                      | Common Solution                        |
| --------------------- | ------------------------------------------ | -------------------------------------- |
| **Cache Stampede**    | Many requests on cache miss                | Locking, cache warming, TTL jitter     |
| **Cache Consistency** | Cache and DB out of sync                   | Cache invalidation, Write-Through, TTL |
| **Cache Penetration** | Requests for nonexistent data              | Bloom Filter, cache null values        |
| **Cache Breakdown**   | Hot key expires                            | Refresh hot keys, distributed lock     |
| **Cache Avalanche**   | Many keys expire together or cache failure | Random TTL, cache clustering           |
| **Cold Cache**        | Empty cache after restart                  | Cache warming, preload data            |

### Questions
1. What caching strategy do you use?	
- "Cache-Aside for reads, Write-Behind for write-heavy logs, Write-Through for user profiles."
2. What eviction algorithm?	
- "LRU by default, LFU for viral content, TTL for freshness."
3. What about cache stampede?	
- "Use mutex locks or probabilistic early expiration."
4. What about non-existent keys?	
- "Cache null values with short TTL or use a Bloom Filter."
5. How do you invalidate?	
- "Delete on update + TTL fallback."


