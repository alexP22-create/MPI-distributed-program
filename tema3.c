#include "mpi.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void print_final_vector(int* v, int n) {
	printf("Rezultat:");
	for (int i = 0; i < n; i++) {
		printf(" %d",v[i]);
	}
	printf("\n");
}

void print_complete_topology(int rank, int coord_parent,int coord_1, int coord_2, int* workers,
	int* workers_1, int* workers_2, int nr_workers, int nr_workers_1, int nr_workers_2) {
	char *final = malloc(1000);
	char* rank_index = malloc(5);
	sprintf(rank_index, "%d", rank);
	strcpy(final, "");
	strcat(final, rank_index);
	strcat(final, " -> 0:");
	if (coord_parent == 0) {
		for(int i = 0; i < nr_workers; i ++) {
			char* index = malloc(5);
			sprintf(index, "%d", workers[i]);
			strcat(final, index);
			strcat(final, ",");
		}
	}
	if (coord_1 == 0) {
		for(int i = 0; i < nr_workers_1; i ++) {
			char* index = malloc(5);
			sprintf(index, "%d", workers_1[i]);
			strcat(final, index);
			strcat(final, ",");
		}
	}
	if (coord_2 == 0) {
		for(int i = 0; i < nr_workers_2; i ++) {
			char* index = malloc(5);
			sprintf(index, "%d", workers_2[i]);
			strcat(final, index);
			strcat(final, ",");
		}
	}
	final[strlen(final)-1] = '\0';
	strcat(final, " 1:");
	if (coord_parent == 1) {
		for(int i = 0; i < nr_workers; i ++) {
			char* index = malloc(5);
			sprintf(index, "%d", workers[i]);
			strcat(final, index);
			strcat(final, ",");
		}
	}
	if (coord_1 == 1) {
		for(int i = 0; i < nr_workers_1; i ++) {
			char* index = malloc(5);
			sprintf(index, "%d", workers_1[i]);
			strcat(final, index);
			strcat(final, ",");
		}
	}
	if (coord_2 == 1) {
		for(int i = 0; i < nr_workers_2; i ++) {
			char* index = malloc(5);
			sprintf(index, "%d", workers_2[i]);
			strcat(final, index);
			strcat(final, ",");
		}
	}
	final[strlen(final)-1] = '\0';
	strcat(final, " 2:");
	if (coord_parent == 2) {
		for(int i = 0; i < nr_workers; i ++) {
			char* index = malloc(5);
			sprintf(index, "%d", workers[i]);
			strcat(final, index);
			strcat(final, ",");
		}
	}
	if (coord_1 == 2) {
		for(int i = 0; i < nr_workers_1; i ++) {
			char* index = malloc(5);
			sprintf(index, "%d", workers_1[i]);
			strcat(final, index);
			strcat(final, ",");
		}
	}
	if (coord_2 == 2) {
		for(int i = 0; i < nr_workers_2; i ++) {
			char* index = malloc(5);
			sprintf(index, "%d", workers_2[i]);
			strcat(final, index);
			strcat(final, ",");
		}
	}
	final[strlen(final)-1] = '\0';
	printf("%s\n", final);

}
int main (int argc, char *argv[])
{
    int procs, rank;
	int n, error;
  
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm coordinators;

	//creez un comunicator numai cu
	MPI_Comm_split(MPI_COMM_WORLD, rank / 3, rank % 3, &coordinators);

	if (sizeof(*argv) > 1) {
		error = atoi(argv[2]);
		printf("%d\n", error);
	}

	if (error == 0) {
		//citire fisiere de coordonatori
		if (rank == 0 || rank == 1 || rank == 2) {
			
			int nr_workers = 0;
			char* cluster_name = malloc(20);
			char* rank_index = malloc(2);
			sprintf(rank_index, "%d", rank);
			strcpy(cluster_name, "cluster");
			strcat(cluster_name, rank_index);
			strcat(cluster_name, ".txt");
			FILE * file = fopen(cluster_name, "rt");
			fscanf(file, "%d", &nr_workers);
			int workers[nr_workers];

			for (int i = 0; i < nr_workers; i ++) {
				fscanf(file, "%d", &workers[i]);

				//informare workers despre coordonatorul lor
				MPI_Send(&rank, 1, MPI_INT, workers[i], 1, MPI_COMM_WORLD);
				printf("M(%d,%d)\n", rank, workers[i]);
			}
			// 1 este primul celalalt coordonator de la care primeste
			// 2 este al doilea celalalt coordonator de la care primeste mesaj
			int nr_workers_1;
			int nr_workers_2;
			int coord_1;
			int coord_2;
			int *workers_1;
			int *workers_2;
			//informare ceilalti coordonatori despre workerii coordonatorului curent
			if (rank == 0) {
				//0 -> 1, 2
				MPI_Send(&rank, 1, MPI_INT, 1, 1, MPI_COMM_WORLD);
				printf("M(0,1)\n");
				MPI_Send(&nr_workers, 1, MPI_INT, 1, 1, MPI_COMM_WORLD);
				printf("M(0,1)\n");
				MPI_Send(&workers, nr_workers, MPI_INT, 1, 1, MPI_COMM_WORLD);
				printf("M(0,1)\n");

				MPI_Send(&rank, 1, MPI_INT, 2, 1, MPI_COMM_WORLD);
				printf("M(0,2)\n");
				MPI_Send(&nr_workers, 1, MPI_INT, 2, 1, MPI_COMM_WORLD);
				printf("M(0,2)\n");
				MPI_Send(&workers, nr_workers, MPI_INT, 2, 1, MPI_COMM_WORLD);
				printf("M(0,2)\n");

				//0 <- 1, 2
				MPI_Status status;
				MPI_Recv(&coord_1, 1, MPI_INT, 1, 1, MPI_COMM_WORLD, &status);
				MPI_Recv(&nr_workers_1, 1, MPI_INT, 1, 1, MPI_COMM_WORLD, &status);
				workers_1 = (int*) calloc(nr_workers_1, sizeof(int));
				MPI_Recv(workers_1, nr_workers_1, MPI_INT, 1, 1, MPI_COMM_WORLD, &status);

				MPI_Recv(&coord_2, 1, MPI_INT, 2, 1, MPI_COMM_WORLD, &status);
				MPI_Recv(&nr_workers_2, 1, MPI_INT, 2, 1, MPI_COMM_WORLD, &status);
				workers_2 = (int*) calloc(nr_workers_2, sizeof(int));
				MPI_Recv(workers_2, nr_workers_2, MPI_INT, 2, 1, MPI_COMM_WORLD, &status);
			}

			if (rank == 1) {
				// 1 <- 0
				MPI_Status status;
				MPI_Recv(&coord_1, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
				MPI_Recv(&nr_workers_1, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
				workers_1 = (int*) calloc(nr_workers_1, sizeof(int));
				MPI_Recv(workers_1, nr_workers_1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);

				//1 <- 2
				MPI_Recv(&coord_2, 1, MPI_INT, 2, 1, MPI_COMM_WORLD, &status);
				MPI_Recv(&nr_workers_2, 1, MPI_INT, 2, 1, MPI_COMM_WORLD, &status);
				workers_2 = (int*) calloc(nr_workers_2, sizeof(int));
				MPI_Recv(workers_2, nr_workers_2, MPI_INT, 2, 1, MPI_COMM_WORLD, &status);

				//1 -> 2
				MPI_Send(&rank, 1, MPI_INT, 2, 1, MPI_COMM_WORLD);
				printf("M(1,2)\n");
				MPI_Send(&nr_workers, 1, MPI_INT, 2, 1, MPI_COMM_WORLD);
				printf("M(1,2)\n");
				MPI_Send(&workers, nr_workers, MPI_INT, 2, 1, MPI_COMM_WORLD);
				printf("M(1,2)\n");

				// 1 -> 0
				MPI_Send(&rank, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
				printf("M(1,0)\n");
				MPI_Send(&nr_workers, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
				printf("M(1,0)\n");
				MPI_Send(&workers, nr_workers, MPI_INT, 0, 1, MPI_COMM_WORLD);
				printf("M(1,0)\n");

			}

			if (rank == 2) {
				// 2 <- 0
				MPI_Status status;
				MPI_Recv(&coord_1, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
				MPI_Recv(&nr_workers_1, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
				workers_1 = (int*) calloc(nr_workers_1, sizeof(int));
				MPI_Recv(workers_1, nr_workers_1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);

				// 2-> 1
				MPI_Send(&rank, 1, MPI_INT, 1, 1, MPI_COMM_WORLD);
				printf("M(2,1)\n");
				MPI_Send(&nr_workers, 1, MPI_INT, 1, 1, MPI_COMM_WORLD);
				printf("M(2,1)\n");
				MPI_Send(&workers, nr_workers, MPI_INT, 1, 1, MPI_COMM_WORLD);
				printf("M(2,1)\n");

				//2 <- 1
				MPI_Recv(&coord_2, 1, MPI_INT, 1, 1, MPI_COMM_WORLD, &status);
				MPI_Recv(&nr_workers_2, 1, MPI_INT, 1, 1, MPI_COMM_WORLD, &status);
				workers_2 = (int*) calloc(nr_workers_2, sizeof(int));
				MPI_Recv(workers_2, nr_workers_2, MPI_INT, 1, 1, MPI_COMM_WORLD, &status);

				// 2->0
				MPI_Send(&rank, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
				printf("M(2,0)\n");
				MPI_Send(&nr_workers, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
				printf("M(2,0)\n");
				MPI_Send(&workers, nr_workers, MPI_INT, 0, 1, MPI_COMM_WORLD);
				printf("M(2,0)\n");

			}

			//s-a completat topologia finala pt procesul curent
			print_complete_topology(rank, rank, coord_1, coord_2, workers, workers_1, workers_2,
				nr_workers, nr_workers_1, nr_workers_2);
			int total_workers = nr_workers + nr_workers_1 + nr_workers_2;
			
			MPI_Barrier(coordinators);

			//informare workeri despre despre colegii lor dar si ceilalti workeri ai celorlalti coordonatori
			for (int i = 0; i < nr_workers; i ++) {
				MPI_Send(&nr_workers, 1, MPI_INT, workers[i], 1, MPI_COMM_WORLD);
				printf("M(%d,%d)\n", rank, workers[i]);
				MPI_Send(&workers, nr_workers, MPI_INT, workers[i], 1, MPI_COMM_WORLD);
				printf("M(%d,%d)\n", rank, workers[i]);

				MPI_Send(&coord_1, 1, MPI_INT, workers[i], 1, MPI_COMM_WORLD);
				printf("M(%d,%d)\n", rank, workers[i]);
				MPI_Send(&nr_workers_1, 1, MPI_INT, workers[i], 1, MPI_COMM_WORLD);
				printf("M(%d,%d)\n", rank, workers[i]);
				MPI_Send(workers_1, nr_workers_1, MPI_INT, workers[i], 1, MPI_COMM_WORLD);
				printf("M(%d,%d)\n", rank, workers[i]);
				
				MPI_Send(&coord_2, 1, MPI_INT, workers[i], 1, MPI_COMM_WORLD);
				printf("M(%d,%d)\n", rank, workers[i]);
				MPI_Send(&nr_workers_2, 1, MPI_INT, workers[i], 1, MPI_COMM_WORLD);
				printf("M(%d,%d)\n", rank, workers[i]);
				MPI_Send(workers_2, nr_workers_2, MPI_INT, workers[i], 1, MPI_COMM_WORLD);
				printf("M(%d,%d)\n", rank, workers[i]);
			}
			//etapa2
			if (rank == 0) {
				n = atoi(argv[1]);
				int v[n];
				for (int i = 0; i < n; i ++) {
					v[i] = i;
				}
				//trimit n si vectorul v la fiecare cluster
				MPI_Send(&n, 1, MPI_INT, coord_1, 1, MPI_COMM_WORLD);
				printf("M(%d,%d)\n", rank, coord_1);
				MPI_Send(&v, n, MPI_INT, coord_1, 1, MPI_COMM_WORLD);
				printf("M(%d,%d)\n", rank, coord_1);

				MPI_Send(&n, 1, MPI_INT, coord_2, 1, MPI_COMM_WORLD);
				printf("M(%d,%d)\n", rank, coord_2);
				MPI_Send(&v, n, MPI_INT, coord_2, 1, MPI_COMM_WORLD);
				printf("M(%d,%d)\n", rank, coord_2);

				//se trimite n fiecarui worker
				for (int i = 0; i < nr_workers; i ++) {
					//se trimit n si v catre workeri
					MPI_Send(&n, 1, MPI_INT, workers[i], 1, MPI_COMM_WORLD);
					printf("M(%d,%d)\n", rank, workers[i]);
					MPI_Send(&v, n, MPI_INT, workers[i], 1, MPI_COMM_WORLD);
					printf("M(%d,%d)\n", rank, workers[i]);
				}
				MPI_Status status;
				//primeste de la fiecare worker
				for (int i = 0; i < nr_workers; i ++) {
					int new_v[n];
					MPI_Recv(&new_v, n, MPI_INT, workers[i], 1, MPI_COMM_WORLD,&status);

					//updateaza v-ul
					for(int j = 0; j < n; j ++) {
						if (v[j] <= new_v[j]) {
							v[j] = new_v[j];
						}
					}
				}

				//primeste de la ceilalti coordonatori
				for (int i = 1; i <= 2; i ++) {
					int new_v[n];
					MPI_Recv(&new_v, n, MPI_INT, i, 1, MPI_COMM_WORLD,&status);

					//updateaza v-ul
					for(int j = 0; j < n; j ++) {
						if (v[j] <= new_v[j]) {
							v[j] = new_v[j];
						}
					}
				}

				print_final_vector(v, n);

			} else {
				//primeste n si array-ul
				MPI_Status status;
				MPI_Recv(&n, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
				int v[n];
				MPI_Recv(&v, n, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);

				//se trimite n fiecarui worker
				for (int i = 0; i < nr_workers; i ++) {
					//se trimit n si v catre workeri
					MPI_Send(&n, 1, MPI_INT, workers[i], 1, MPI_COMM_WORLD);
					printf("M(%d,%d)\n", rank, workers[i]);
					MPI_Send(&v, n, MPI_INT, workers[i], 1, MPI_COMM_WORLD);
					printf("M(%d,%d)\n", rank, workers[i]);
				}

				//primeste de la fiecare worker
				for (int i = 0; i < nr_workers; i ++) {
					int new_v[n];
					MPI_Recv(&new_v, n, MPI_INT, workers[i], 1, MPI_COMM_WORLD,&status);

					//updateaza v-ul
					for(int j = 0; j < n; j ++) {
						if (v[j] <= new_v[j]) {
							v[j] = new_v[j];
						}
					}
				}

				//trimite la 0
				MPI_Send(&v, n, MPI_INT, 0, 1, MPI_COMM_WORLD);
				printf("M(%d,0)\n", rank);		
			}

		}
		//workeri
		else {
			//coordonatorul curent
			int my_coordonator;
			MPI_Status status;
			MPI_Recv(&my_coordonator, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD,&status);

			//colegii workerului
			int nr;
			MPI_Recv(&nr, 1, MPI_INT, my_coordonator, 1, MPI_COMM_WORLD,&status);
			int collegues[nr];
			MPI_Recv(&collegues, nr, MPI_INT, my_coordonator, 1, MPI_COMM_WORLD, &status);

			//workerii coordonatorului 1
			int coord_1;
			int nr_1;
			MPI_Recv(&coord_1, 1, MPI_INT, my_coordonator, 1, MPI_COMM_WORLD,&status);
			MPI_Recv(&nr_1, 1, MPI_INT, my_coordonator, 1, MPI_COMM_WORLD,&status);
			int workers_1[nr_1];
			MPI_Recv(&workers_1, nr_1, MPI_INT, my_coordonator, 1, MPI_COMM_WORLD, &status);

			//workerii coordonatorului 2
			int coord_2;
			int nr_2;
			MPI_Recv(&coord_2, 1, MPI_INT, my_coordonator, 1, MPI_COMM_WORLD,&status);
			MPI_Recv(&nr_2, 1, MPI_INT, my_coordonator, 1, MPI_COMM_WORLD,&status);
			int workers_2[nr_2];
			MPI_Recv(&workers_2, nr_2, MPI_INT, my_coordonator, 1, MPI_COMM_WORLD, &status);

			print_complete_topology(rank, my_coordonator, coord_1, coord_2, collegues, workers_1, workers_2, nr, nr_1, nr_2);

			//primesc n
			int n;
			MPI_Recv(&n, 1, MPI_INT, my_coordonator, 1, MPI_COMM_WORLD, &status);
			//primesc v
			int v[n];
			MPI_Recv(&v, n, MPI_INT, my_coordonator, 1, MPI_COMM_WORLD, &status);

			int position = -1;
			for(int i = 0; i < nr; i++) {
				if (collegues[i] == rank) {
					position = i;
				}
			}
			
			if (my_coordonator == 1) {
				int before_workers = 0;
				if (coord_1 == 0) {
					before_workers = nr_1;
				}
				if (coord_2 == 0) {
					before_workers = nr_2;
				}
				position += before_workers; 
			} 

			if (my_coordonator == 2) {
				int before_workers = nr_1 + nr_2;
				
				position += before_workers; 
			} 
			int total_workers = nr + nr_1 + nr_2;
			
			//dubleaza elementele din vector care corespund pentru acest worker
			while (position < n) {
				v[position] *= 2;
				position += total_workers;
			}
			
			//trimitere inapoi la coordonator
			MPI_Send(&v, n, MPI_INT, my_coordonator, 1, MPI_COMM_WORLD);
			printf("M(%d,%d)\n", rank, my_coordonator);
			
		}
	} else if (error==1){
		//bonus

		//citire fisiere de coordonatori
		if (rank == 0 || rank == 1 || rank == 2) {
			
			int nr_workers = 0;
			char* cluster_name = malloc(20);
			char* rank_index = malloc(2);
			sprintf(rank_index, "%d", rank);
			strcpy(cluster_name, "cluster");
			strcat(cluster_name, rank_index);
			strcat(cluster_name, ".txt");
			FILE * file = fopen(cluster_name, "rt");
			fscanf(file, "%d", &nr_workers);
			int workers[nr_workers];

			for (int i = 0; i < nr_workers; i ++) {
				fscanf(file, "%d", &workers[i]);

				//informare workers despre coordonatorul lor
				MPI_Send(&rank, 1, MPI_INT, workers[i], 1, MPI_COMM_WORLD);
				printf("M(%d,%d)\n", rank, workers[i]);
			}
			// 1 este primul cellalat coordonator de la care primeste
			// 2 este al doilea celallat coordonator de la care primeste mesaj
			int nr_workers_1;
			int nr_workers_2;
			int coord_1;
			int coord_2;
			int *workers_1;
			int *workers_2;
			//informare ceilalti coordonatori despre workerii coordonatorului curent
			if (rank == 0) {
				//0 ->2
				MPI_Send(&rank, 1, MPI_INT, 2, 1, MPI_COMM_WORLD);
				printf("M(0,2)\n");
				MPI_Send(&nr_workers, 1, MPI_INT, 2, 1, MPI_COMM_WORLD);
				printf("M(0,2)\n");
				MPI_Send(&workers, nr_workers, MPI_INT, 2, 1, MPI_COMM_WORLD);
				printf("M(0,2)\n");

				//0 <- 2 pt 1 si 2
				MPI_Status status;
				MPI_Recv(&coord_1, 1, MPI_INT, 2, 1, MPI_COMM_WORLD, &status);
				MPI_Recv(&nr_workers_1, 1, MPI_INT, 2, 1, MPI_COMM_WORLD, &status);
				workers_1 = (int*) calloc(nr_workers_1, sizeof(int));
				MPI_Recv(workers_1, nr_workers_1, MPI_INT, 2, 1, MPI_COMM_WORLD, &status);

				MPI_Recv(&coord_2, 1, MPI_INT, 2, 1, MPI_COMM_WORLD, &status);
				MPI_Recv(&nr_workers_2, 1, MPI_INT, 2, 1, MPI_COMM_WORLD, &status);
				workers_2 = (int*) calloc(nr_workers_2, sizeof(int));
				MPI_Recv(workers_2, nr_workers_2, MPI_INT, 2, 1, MPI_COMM_WORLD, &status);
			}

			if (rank == 1) {
				// 1 <- 2
				MPI_Status status;
				MPI_Recv(&coord_1, 1, MPI_INT, 2, 1, MPI_COMM_WORLD, &status);
				MPI_Recv(&nr_workers_1, 1, MPI_INT, 2, 1, MPI_COMM_WORLD, &status);
				workers_1 = (int*) calloc(nr_workers_1, sizeof(int));
				MPI_Recv(workers_1, nr_workers_1, MPI_INT, 2, 1, MPI_COMM_WORLD, &status);

				//1 <- 2
				MPI_Recv(&coord_2, 1, MPI_INT, 2, 1, MPI_COMM_WORLD, &status);
				MPI_Recv(&nr_workers_2, 1, MPI_INT, 2, 1, MPI_COMM_WORLD, &status);
				workers_2 = (int*) calloc(nr_workers_2, sizeof(int));
				MPI_Recv(workers_2, nr_workers_2, MPI_INT, 2, 1, MPI_COMM_WORLD, &status);

				//1 -> 2
				MPI_Send(&rank, 1, MPI_INT, 2, 1, MPI_COMM_WORLD);
				printf("M(1,2)\n");
				MPI_Send(&nr_workers, 1, MPI_INT, 2, 1, MPI_COMM_WORLD);
				printf("M(1,2)\n");
				MPI_Send(&workers, nr_workers, MPI_INT, 2, 1, MPI_COMM_WORLD);
				printf("M(1,2)\n");

			}

			if (rank == 2) {
				// 2 <- 0
				MPI_Status status;
				MPI_Recv(&coord_1, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
				MPI_Recv(&nr_workers_1, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
				workers_1 = (int*) calloc(nr_workers_1, sizeof(int));
				MPI_Recv(workers_1, nr_workers_1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);

				// 2-> 1 pt 2
				MPI_Send(&rank, 1, MPI_INT, 1, 1, MPI_COMM_WORLD);
				printf("M(2,1)\n");
				MPI_Send(&nr_workers, 1, MPI_INT, 1, 1, MPI_COMM_WORLD);
				printf("M(2,1)\n");
				MPI_Send(&workers, nr_workers, MPI_INT, 1, 1, MPI_COMM_WORLD);
				printf("M(2,1)\n");

				// 2->1 pt 0
				MPI_Send(&coord_1, 1, MPI_INT, 1, 1, MPI_COMM_WORLD);
				printf("M(2,1)\n");
				MPI_Send(&nr_workers_1, 1, MPI_INT, 1, 1, MPI_COMM_WORLD);
				printf("M(2,1)\n");
				MPI_Send(workers_1, nr_workers_1, MPI_INT, 1, 1, MPI_COMM_WORLD);
				printf("M(2,1)\n");

				//2 <- 1
				MPI_Recv(&coord_2, 1, MPI_INT, 1, 1, MPI_COMM_WORLD, &status);
				MPI_Recv(&nr_workers_2, 1, MPI_INT, 1, 1, MPI_COMM_WORLD, &status);
				workers_2 = (int*) calloc(nr_workers_2, sizeof(int));
				MPI_Recv(workers_2, nr_workers_2, MPI_INT, 1, 1, MPI_COMM_WORLD, &status);

				// 2->0 pt 1
				MPI_Send(&coord_2, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
				printf("M(2,0)\n");
				MPI_Send(&nr_workers_2, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
				printf("M(2,0)\n");
				MPI_Send(workers_2, nr_workers_2, MPI_INT, 0, 1, MPI_COMM_WORLD);
				printf("M(2,0)\n");

				//2->0 pt 0
				MPI_Send(&rank, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
				printf("M(2,0)\n");
				MPI_Send(&nr_workers, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
				printf("M(2,0)\n");
				MPI_Send(&workers, nr_workers, MPI_INT, 0, 1, MPI_COMM_WORLD);
				printf("M(2,0)\n");

			}

			//s-a completat topologia finala pt procesul curent
			print_complete_topology(rank, rank, coord_1, coord_2, workers, workers_1, workers_2,
				nr_workers, nr_workers_1, nr_workers_2);
			int total_workers = nr_workers + nr_workers_1 + nr_workers_2;
			
			MPI_Barrier(coordinators);

			//informare workeri despre despre colegii lor dar si ceilalti workeri ai celorlalti coordonatori
			for (int i = 0; i < nr_workers; i ++) {
				MPI_Send(&nr_workers, 1, MPI_INT, workers[i], 1, MPI_COMM_WORLD);
				printf("M(%d,%d)\n", rank, workers[i]);
				MPI_Send(&workers, nr_workers, MPI_INT, workers[i], 1, MPI_COMM_WORLD);
				printf("M(%d,%d)\n", rank, workers[i]);

				MPI_Send(&coord_1, 1, MPI_INT, workers[i], 1, MPI_COMM_WORLD);
				printf("M(%d,%d)\n", rank, workers[i]);
				MPI_Send(&nr_workers_1, 1, MPI_INT, workers[i], 1, MPI_COMM_WORLD);
				printf("M(%d,%d)\n", rank, workers[i]);
				MPI_Send(workers_1, nr_workers_1, MPI_INT, workers[i], 1, MPI_COMM_WORLD);
				printf("M(%d,%d)\n", rank, workers[i]);
				
				MPI_Send(&coord_2, 1, MPI_INT, workers[i], 1, MPI_COMM_WORLD);
				printf("M(%d,%d)\n", rank, workers[i]);
				MPI_Send(&nr_workers_2, 1, MPI_INT, workers[i], 1, MPI_COMM_WORLD);
				printf("M(%d,%d)\n", rank, workers[i]);
				MPI_Send(workers_2, nr_workers_2, MPI_INT, workers[i], 1, MPI_COMM_WORLD);
				printf("M(%d,%d)\n", rank, workers[i]);
			}
			if (rank == 0) {
				n = atoi(argv[1]);
				int v[n];
				for (int i = 0; i < n; i ++) {
					v[i] = i;
				}
				//trimit n si vectorul v la coordonator 2
				MPI_Send(&n, 1, MPI_INT, 2, 1, MPI_COMM_WORLD);
				printf("M(%d,2)\n", rank);
				MPI_Send(&v, n, MPI_INT, 2, 1, MPI_COMM_WORLD);
				printf("M(%d,2)\n", rank);

				//se trimite n fiecarui worker
				for (int i = 0; i < nr_workers; i ++) {
					//se trimit n si v catre workeri
					MPI_Send(&n, 1, MPI_INT, workers[i], 1, MPI_COMM_WORLD);
					printf("M(%d,%d)\n", rank, workers[i]);
					MPI_Send(&v, n, MPI_INT, workers[i], 1, MPI_COMM_WORLD);
					printf("M(%d,%d)\n", rank, workers[i]);
				}
				MPI_Status status;
				//primeste de la fiecare worker
				for (int i = 0; i < nr_workers; i ++) {
					int new_v[n];
					MPI_Recv(&new_v, n, MPI_INT, workers[i], 1, MPI_COMM_WORLD,&status);

					//updateaza v-ul
					for(int j = 0; j < n; j ++) {
						if (v[j] <= new_v[j]) {
							v[j] = new_v[j];
						}
					}
				}

				//primeste de la cluster 2
				int new_v[n];
				MPI_Recv(&new_v, n, MPI_INT, 2, 1, MPI_COMM_WORLD,&status);

				//updateaza v-ul
				for(int j = 0; j < n; j ++) {
					if (v[j] <= new_v[j]) {
						v[j] = new_v[j];
					}
				}

				print_final_vector(v, n);

			} else {
				if (rank == 2) {
					//primeste n si array-ul de la 0
					MPI_Status status;
					MPI_Recv(&n, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
					int v[n];
					MPI_Recv(&v, n, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);

					//trimite la 1
					MPI_Send(&n, 1, MPI_INT, 1, 1, MPI_COMM_WORLD);
					printf("M(2,1)\n");
					MPI_Send(&v, n, MPI_INT, 1, 1, MPI_COMM_WORLD);
					printf("M(2,1)\n");
					
					//se trimite n fiecarui worker
					for (int i = 0; i < nr_workers; i ++) {
						//se trimit n si v catre workeri
						MPI_Send(&n, 1, MPI_INT, workers[i], 1, MPI_COMM_WORLD);
						printf("M(%d,%d)\n", rank, workers[i]);
						MPI_Send(&v, n, MPI_INT, workers[i], 1, MPI_COMM_WORLD);
						printf("M(%d,%d)\n", rank, workers[i]);
					}

					//primeste de la fiecare worker
					for (int i = 0; i < nr_workers; i ++) {
						int new_v[n];
						MPI_Recv(&new_v, n, MPI_INT, workers[i], 1, MPI_COMM_WORLD,&status);

						//updateaza v-ul
						for(int j = 0; j < n; j ++) {
							if (v[j] <= new_v[j]) {
								v[j] = new_v[j];
							}
						}
					}

					//primeste de la 1
					int new_v[n];
					MPI_Recv(&new_v, n, MPI_INT, 1, 1, MPI_COMM_WORLD,&status);

					//updateaza v-ul
					for(int j = 0; j < n; j ++) {
						if (v[j] <= new_v[j]) {
							v[j] = new_v[j];
						}
					}

					//trimite la 0
					MPI_Send(&v, n, MPI_INT, 0, 1, MPI_COMM_WORLD);
					printf("M(2,0)\n");		
				}

				if (rank == 1) {
					//primeste n si array-ul de la 2
					MPI_Status status;
					MPI_Recv(&n, 1, MPI_INT, 2, 1, MPI_COMM_WORLD, &status);
					int v[n];
					MPI_Recv(&v, n, MPI_INT, 2, 1, MPI_COMM_WORLD, &status);
					
					//se trimite n fiecarui worker
					for (int i = 0; i < nr_workers; i ++) {
						//se trimit n si v catre workeri
						MPI_Send(&n, 1, MPI_INT, workers[i], 1, MPI_COMM_WORLD);
						printf("M(%d,%d)\n", rank, workers[i]);
						MPI_Send(&v, n, MPI_INT, workers[i], 1, MPI_COMM_WORLD);
						printf("M(%d,%d)\n", rank, workers[i]);
					}

					//primeste de la fiecare worker
					for (int i = 0; i < nr_workers; i ++) {
						int new_v[n];
						MPI_Recv(&new_v, n, MPI_INT, workers[i], 1, MPI_COMM_WORLD,&status);

						//updateaza v-ul
						for(int j = 0; j < n; j ++) {
							if (v[j] <= new_v[j]) {
								v[j] = new_v[j];
							}
						}
					}

					//trimite la 2
					MPI_Send(&v, n, MPI_INT, 2, 1, MPI_COMM_WORLD);
					printf("M(%d,2)\n", rank);
				}
			}

		}
		//workeri
		else {
			//coordonatorul curent
			int my_coordonator;
			MPI_Status status;
			MPI_Recv(&my_coordonator, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD,&status);

			//colegii workerului
			int nr;
			MPI_Recv(&nr, 1, MPI_INT, my_coordonator, 1, MPI_COMM_WORLD,&status);
			int collegues[nr];
			MPI_Recv(&collegues, nr, MPI_INT, my_coordonator, 1, MPI_COMM_WORLD, &status);

			//workerii coordonatorului 1
			int coord_1;
			int nr_1;
			MPI_Recv(&coord_1, 1, MPI_INT, my_coordonator, 1, MPI_COMM_WORLD,&status);
			MPI_Recv(&nr_1, 1, MPI_INT, my_coordonator, 1, MPI_COMM_WORLD,&status);
			int workers_1[nr_1];
			MPI_Recv(&workers_1, nr_1, MPI_INT, my_coordonator, 1, MPI_COMM_WORLD, &status);

			//workerii coordonatorului 2
			int coord_2;
			int nr_2;
			MPI_Recv(&coord_2, 1, MPI_INT, my_coordonator, 1, MPI_COMM_WORLD,&status);
			MPI_Recv(&nr_2, 1, MPI_INT, my_coordonator, 1, MPI_COMM_WORLD,&status);
			int workers_2[nr_2];
			MPI_Recv(&workers_2, nr_2, MPI_INT, my_coordonator, 1, MPI_COMM_WORLD, &status);

			print_complete_topology(rank, my_coordonator, coord_1, coord_2, collegues, workers_1, workers_2, nr, nr_1, nr_2);

			//primesc n
			int n;
			MPI_Recv(&n, 1, MPI_INT, my_coordonator, 1, MPI_COMM_WORLD, &status);
			//primesc v
			int v[n];
			MPI_Recv(&v, n, MPI_INT, my_coordonator, 1, MPI_COMM_WORLD, &status);

			int position = -1;
			for(int i = 0; i < nr; i++) {
				if (collegues[i] == rank) {
					position = i;
				}
			}
			
			if (my_coordonator == 1) {
				int before_workers = 0;
				if (coord_1 == 0) {
					before_workers = nr_1;
				}
				if (coord_2 == 0) {
					before_workers = nr_2;
				}
				position += before_workers; 
			} 

			if (my_coordonator == 2) {
				int before_workers = nr_1 + nr_2;
				
				position += before_workers; 
			} 
			int total_workers = nr + nr_1 + nr_2;
			
			//dubleaza elementele din vector care corespund pentru acest worker
			while (position < n) {
				v[position] *= 2;
				position += total_workers;
			}
			
			//trimitere inapoi la coordonator
			MPI_Send(&v, n, MPI_INT, my_coordonator, 1, MPI_COMM_WORLD);
			printf("M(%d,%d)\n", rank, my_coordonator);
			
		}
	}

    MPI_Finalize();

}
