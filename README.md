# Process synchronization
An algorith made to emulate the operation of a printer (synchronizing the file that will be impressed).
The synchronization is made using **semaphores** from the `pthread.h` library that will manage 2 buffers (main and secondary). Each file will be represented as a thread and will enter in one of the buffers (if it has free space). 

*Administrative files will enter in the main buffer while diverse files will go to the secondary buffer. 

# How to use
1. Compile `sync.c` using:
```
gcc -pthread sync.c -o sync
```
2. Now, execute the `sync` file. You'll need to pass the buffer size (both will have the same size), the number of administrative files (this files are prioritized) and the number of diverse files. Ex.:
```
./threads 5 3 4 
```
`5` being the size of both buffers, `3` being the number of adm files and `4` being the number of diverse files.

3. The output for the above command would be:
```
        -------------------------------------------

        File being printed: [140446268483328]
        File type: administrative
        Estimated time: 2s

        Files in the main buffer
        -----------
        Slot 0 - [140446268483328] 
        Slot 1 - [140446260090624] 
        Slot 2 - [140446251697920] 
        Slot 3 - [0] 
        Slot 4 - [0] 

        Files in the secondary buffer
        -----------
        Slot 0 - [140446168774400] 
        Slot 1 - [0] 
        Slot 2 - [0] 
        Slot 3 - [0] 
        Slot 4 - [0] 

        The file [140446268483328] was successfully printed!

        -------------------------------------------
        
                        ...
                        
        Files in the main buffer
        -----------
        Slot 0 - [0] 
        Slot 1 - [0] 
        Slot 2 - [0] 
        Slot 3 - [0] 
        Slot 4 - [0] 

        Files in the secondary buffer
        -----------
        Slot 0 - [0] 
        Slot 1 - [0] 
        Slot 2 - [0] 
        Slot 3 - [0] 
        Slot 4 - [0] 

        -------------------------------------------
        Administrative files printed: [3]
        Diverse files printed: [4]
        Total: [7]
        Execution time: 12s
        -------------------------------------------          

```
