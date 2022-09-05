# CacheMoney

## SUMMARY:
- Multithreaded TCP server exposed on port 8080 that can be communicated with the inbuilt client binary.
- We use protobuf to serialize and deserialize data over the wire.
- Behind the communication lives a thread-safe LRU Cache that runs the instructions from the data recieved over the wire.

## DONE:
- LRU based cache.
- Make thread safe.
- Use Makefile.
- TCP Server
- Protobuf based serilization and deserialization across wire(TCP)
- Create Multithreaded entrypoint and api's for external processes to interact

## NOTE:
- Thank you to everyone on stack overflow who had examples for how to work with protobuf over TCP in C++ I thought I would die trying to figure it out.
