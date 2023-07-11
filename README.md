# DxHash
## Abstract
Consistent Hashing (CH) algorithms are widely adopted in networks and distributed systems for their ability to achieve load balancing and minimize disruptions. However, the rise of the Internet of Things (IoT) has introduced new challenges for existing CH algorithms, characterized by high memory usage and update overhead. This paper presents DxHash, a novel CH algorithm based on repeated pseudo-random number generation. DxHash offers significant benefits, including a remarkably low memory footprint, high lookup throughput, and minimal update overhead. Additionally, we introduce a weighted variant of DxHash, enabling adaptable weight adjustments to handle heterogeneous load distribution. Through extensive evaluation, we demonstrate that DxHash outperforms AnchorHash, a state-of-the-art CH algorithm, in terms of the reduction of up to 98.4\% in memory footprint and comparable performance in lookup and update. 
## Code
We provide the c++ implementation of 4 consistent hashing algorithms, including **Karger Ring**, **MaglevHash**, **AnchorHash** and **DxHash**. Besides, a simple performance test is given. 
## Note
We refer the c++ implementation of [AnchorHash](https://github.com/anchorhash/cpp-anchorhash), which makes use of the CRC32 CPU instruction of the Streaming SIMD Extensions 4 (SSE4). You can replace it in src/crc32c_sse42_u64.h.
