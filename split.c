#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void splitFile(const char *srcFile, size_t partSize) {
    int fdSrc = open(srcFile, O_RDONLY);
    if (fdSrc == -1) {
        perror("ERREUR: Ouverture du fichier source");
        exit(1);
    }

    // Récupérer la taille du fichier source
    struct stat stFichierSrc;
    int result = fstat(fdSrc, &stFichierSrc);
    if (result == -1) {
        perror("ERREUR: Récupération des informations du fichier source");
        close(fdSrc);
        exit(1);
    }

    off_t fileSize = stFichierSrc.st_size;
    int partNum = 1;
    char buffer[1024];
    ssize_t bytesRead, bytesWritten;

    while (fileSize > 0) {
        // Créer un fichier pour chaque partie
        char partName[256];
        snprintf(partName, sizeof(partName), "%s.part%d", srcFile, partNum);
        int fdDst = open(partName, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fdDst == -1) {
            perror("ERREUR: Création du fichier de destination");
            close(fdSrc);
            exit(1);
        }

        // Lire et écrire dans le fichier de destination
        size_t bytesToRead = partSize < fileSize ? partSize : fileSize;
        while (bytesToRead > 0) {
            bytesRead = read(fdSrc, buffer, sizeof(buffer));
            if (bytesRead <= 0) {
                break;
            }

            bytesWritten = write(fdDst, buffer, bytesRead);
            if (bytesWritten != bytesRead) {
                perror("ERREUR: Écriture dans le fichier de destination");
                close(fdSrc);
                close(fdDst);
                exit(1);
            }
            bytesToRead -= bytesWritten;
        }

        close(fdDst);
        fileSize -= bytesToRead;
        partNum++;
    }

    close(fdSrc);
    printf("Fichier découpé avec succès en %d parties.\n", partNum - 1);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        perror("Utilisation : ./split nom_fichier taille_max");
        return 1;
    }

    const char *srcFile = argv[1];
    size_t partSize = atoi(argv[2]);

    if (partSize <= 0) {
        perror("La taille maximale de la partie doit être un entier positif.");
        return 1;
    }

    splitFile(srcFile, partSize);
    return 0;
}













