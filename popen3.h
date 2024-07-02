#ifndef POPEN3_H
#define POPEN3_H

#include <stddef.h>

#define POPEN3_VERSION_MAJOR 1
#define POPEN3_VERSION_MINOR 0
#define POPEN3_VERSION_PATCH 0

#ifdef __cplusplus
extern "C" {
#endif

typedef struct popen3_s popen3_t;

/**
 * @brief Open program by creating a pipe.
 * @param[out] pip - I/O stream for stdin/stdout/stderr.
 * @param[in] file - The file path to execute. If set to NULL, take the first
 *   string from \p argv as the file path.
 * @param[in] cwd - The working directory. Set to NULL to keep default.
 * @param[in] argv - Command line arguments passed to the file.
 * @param[in] envp - Environments passed to the file. Set to NULL to clear the
 *   environments. To stay the same as host, use `environ`.
 * @param[in] type - a null‐terminated string which must contain at least one
 *   letter. 'i' for open stdin, 'o' for open stdout, 'e' for open stderr.
 * @return 0 - Open success.
 * @return <0 - A negative number for errno. The abs value is the actual system
 *   error code.
 */
int popen3(popen3_t** pip, const char* file, const char* cwd,
    const char* const argv[], const char* const envp[],  const char* type);

/**
 * @brief Wait for program exit and close pipe.
 * @param[in] pip - I/O stream.
 * @return >=0 - The program terminated normally, the return code is the exit code.
 * @return <0 - The program terminated out of except. -1 is returned.
 */
int popen3_close(popen3_t* pip);

/**
 * @brief Close stdin pipe.
 * @param[in] pip - I/O stream.
 * @return 0 - Close success.
 * @return <0 - Errno. The abs value is the errno.
 */
int popen3_shutdown_stdin(popen3_t* pip);

/**
 * @brief Close stdout pipe.
 * @param[in] pip - I/O stream.
 * @return 0 - Close success.
 * @return <0 - Errno. The abs value is the errno.
 */
int popen3_shutdown_stdout(popen3_t* pip);

/**
 * @brief Close stderr pipe.
 * @param[in] pip - I/O stream.
 * @return 0 - Close success.
 * @return <0 - Errno. The abs value is the errno.
 */
int popen3_shutdown_stderr(popen3_t* pip);

/**
 * @brief Write to program stdin in non-block mode.
 * @param[in] pip - I/O stream.
 * @param[in] data - Data to write.
 * @param[in] size - The bytes of data to write.
 * @return >=0 - The number of bytes written.
 * @return <0 - A negative number of errno. The abs value is the actual system
 *   error code.
 */
int popen3_stdin(popen3_t* pip, const void* data, size_t size);

/**
 * @brief Read from program stdout in non-block mode.
 * @param[in] pip - I/O stream.
 * @param[out] data - Buffer to store data.
 * @param[in] size - The size of buffer.
 * @return >=0 - The number of bytes read.
 * @return <0 - A negative number of errno. The abs value is the actual system
 *   error code.
 */
int popen3_stdout(popen3_t* pip, void* buff, size_t size);

/**
 * @brief Read from program stderr in non-block mode.
 * @param[in] pip - I/O stream.
 * @param[out] data - Buffer to store data.
 * @param[in] size - The size of buffer.
 * @return >=0 - The number of bytes read.
 * @return <0 - A negative number of errno. The abs value is the actual system
 *   error code.
 */
int popen3_stderr(popen3_t* pip, void* buff, size_t size);

#ifdef __cplusplus
}
#endif
#endif
