#include "lib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	bool ok;
	int i; 

	printf("\t\tThe queries correctly executed from the file Query.txt will now be printed: \n\n");

    FILE* file = fopen("queries.txt", "r");
    char line[256];
    while (fgets(line, sizeof(line), file)) {
    	line[strcspn(line, "\n")] = 0;
		ok = executeQuery(line);
		if (ok == true)
			printf("%s\n", line); 	
    }
    fclose(file);

	//getchar();
	return 0;
}
