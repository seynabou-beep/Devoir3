#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void joinFiles(int numParts, char *parts[], const char *dstFile) {
    int fdDst = open(dstFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fdDst == -1) {
        perror("ERREUR: Création du fichier de destination");
        exit(1);
    }

    char buffer[1024];
    ssize_t bytesRead, bytesWritten;

    for (int i = 0; i < numParts; i++) {
        int fdSrc = open(parts[i], O_RDONLY);
        if (fdSrc == -1) {
            perror("ERREUR: Ouverture du fichier source");
            close(fdDst);
            exit(1);
        }

        while ((bytesRead = read(fdSrc, buffer, sizeof(buffer))) > 0) {
            bytesWritten = write(fdDst, buffer, bytesRead);
            if (bytesWritten != bytesRead) {
                perror("ERREUR: Écriture dans le fichier de destination");
                close(fdSrc);
                close(fdDst);
                exit(1);
            }
        }

        close(fdSrc);
    }

    close(fdDst);
    printf("Fichiers joints avec succès dans %s\n", dstFile);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        perror("Utilisation : ./join fichier_de_destination part1 part2 ...");
        return 1;
    }

    const char *dstFile = argv[1];
    int numParts = argc - 2;
    char *parts[numParts];

    for (int i = 0; i < numParts; i++) {
        parts[i] = argv[i + 2];
    }

    joinFiles(numParts, parts, dstFile);
    return 0;
}

//on execute le programme comme suit:
//  ./join Nouveaufichier  part1 part2...  sachant que nouveaufichier est le nouveau fichier 
//qui contiendra les diff parties decoupees;


