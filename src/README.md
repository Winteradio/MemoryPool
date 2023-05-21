1. Include necessary headers:

    The code begins by including the necessary headers, including "MemoryManager.h" and "Log.h", which are essential for using the Memory Manager and logging system.

2.Define the WinMain function:

    The WinMain function serves as the entry point for a Windows application. It demonstrates the usage of the Memory Manager and provides an example scenario.

3.Initialize the Memory Manager:

    The Memory Manager is initialized by calling MemoryManager::GetHandle(), which returns a pointer to the singleton instance of the Memory Manager. This instance will be used throughout the application.

4.Set the default size for memory pools:

    The code sets the default size for memory pools using MemoryManager::SetDefaultSize(size). This determines the total size of memory available for each memory pool. In this example, the default size is set to 256.

5.Allocate objects from memory pools:

    The code demonstrates how to allocate objects from memory pools. It uses the Allocate<T>(args...) method of the Memory Manager.
    For example, int* intPtr = MemoryManager::GetHandle()->Allocate<int>(42) allocates an integer object with a value of 42 from the memory pool for int objects. The allocated memory address is stored in the intPtr variable.
    Similarly, float* floatPtr = MemoryManager::GetHandle()->Allocate<float>(3.14f) allocates a float object with a value of 3.14 from the memory pool for float objects.

6.Log messages:

    The code demonstrates how to log messages using the Log class.
    or example, Log::Info("Memory allocated successfully!") logs an informational message to the console.

7.Deallocate objects from memory pools:

    The code shows how to deallocate objects from memory pools using the Deallocate<T>(object) method of the Memory Manager.
    For instance, MemoryManager::GetHandle()->Deallocate<int>(intPtr) deallocates the previously allocated int object and makes the memory available for reuse.

8. Destroy the Memory Manager:

    Finally, the Memory Manager is destroyed and all allocated memory is released by calling MemoryManager::Destroy()