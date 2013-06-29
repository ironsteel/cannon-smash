#ifndef __getopt_h__INCLUDED__
#define __getopt_h__INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

extern int opterr, optind, optopt, optreset;
extern char *optarg;
int getopt(int argc, char *const *argv, const char *option);

#ifdef __cplusplus
}
#endif

#endif /* __getopt_h__INCLUDED__ */
