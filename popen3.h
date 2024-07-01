#ifndef POPEN3_H
#define POPEN3_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct popen3_s popen3_t;

/**
 * @brief Open program by creating a pipe.
 * @param[out] pip - I/O stream for stdin/stdout/stderr.
 * @param[in] cmd - a pointer to a null‐terminated string containing a shell
 *   command line.
 * @param[in] type - a null‐terminated string which must contain at least one
 *   letter. 'i' for open stdin, 'o' for open stdout, 'e' for open stderr.
 * @return 0 - Open success.
 * @return <0 - A negative number for errno. The abs value is the actual system
 *   error code.
 */
int popen3(popen3_t** pip, const char* cmd, const char* type);

/**
 * @brief Wait for program exit and close pipe.
 * @param[in] pip - I/O stream.
 * @return >=0 - The program terminated normally, the return code is the exit code.
 * @return <0 - The program terminated out of except. -1 is returned.
 */
int popen3_close(popen3_t* pip);

void popen3_shutdown_stdin(popen3_t* pip);
void popen3_shutdown_stdout(popen3_t* pip);
void popen3_shutdown_stderr(popen3_t* pip);

/**
 * @brief Write to program stdin.
 * @param[in] pip - I/O stream.
 * @param[in] data - Data to write.
 * @param[in] size - The bytes of data to write.
 * @return >=0 - The number of bytes written.
 * @return <0 - A negative number of errno. The abs value is the actual system
 *   error code.
 */
int popen3_stdin(popen3_t* pip, const void* data, size_t size);

/**
 * @brief Read from program stdout.
 * @param[in] pip - I/O stream.
 * @param[out] data - Buffer to store data.
 * @param[in] size - The size of buffer.
 * @return >=0 - The number of bytes read.
 * @return <0 - A negative number of errno. The abs value is the actual system
 *   error code.
 */
int popen3_stdout(popen3_t* pip, void* buff, size_t size);

/**
 * @brief Read from program stderr.
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
