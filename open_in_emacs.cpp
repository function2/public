// Michael Seyfert <michael@codesand.org>
// Read from stdin and open the data in emacs (from a temp file).
// This program can be done in a few lines bash.

#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>

using namespace std;

// OpenBSD.
// static const char *EMACS = "/usr/local/bin/emacsclient";
// Debian GNU/Linux.
static const char *EMACS = "/usr/bin/emacsclient";

static const int BUFSIZE=1<<16; // 0x10000 = 64 kb.

int main(int argc, const char *argv[])
{
   vector<unsigned char> buf(BUFSIZE);

   // Open temporary file.
   // Must be able to modify 'template' variable.
   char templat[] = "/tmp/open_in_emacs.XXXXXX";
   int fd;
   fd = mkstemp(templat);
   if(fd == -1){
      fprintf(stderr, "%s: mkstemp failed: %s\n", argv[0],strerror(errno));
      return EXIT_FAILURE;
   }
   FILE *tmp = fdopen(fd,"w");

   while(int c = fread(&buf[0], 1, BUFSIZE, stdin))
      fwrite(&buf[0], c,1, tmp);

   fclose(tmp);
   int child = fork();
   if(!child){
      execl(EMACS, "emacsclient",templat,NULL);
      fprintf(stderr,"%s: Failed to run emacs (%s).\n",argv[0], EMACS);
      return EXIT_FAILURE;
   }
   wait(NULL);
   remove(templat);
   return EXIT_SUCCESS;
}
