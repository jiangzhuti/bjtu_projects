#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <errno.h>
#include <string.h>

#include "dl_lib/dl_common.h"
#include "dl_lib/dl_vector.h"

#define DELTA 0.5

struct point_t {
    double x, y;
};

DL_VECTOR_TEMPLATE(point_p_vector, struct point_t *)

struct cluster_t {
    DL_VECTOR(point_p_vector, points)
//    int id;
//    int label;
};


void random_with_range_n(int min, int max, int n, OUT int *rands)
{
    int i = 0, j = 0;
    int tmp;
    int flag = 1;
    srand(time(0));
    rands[0] = min + rand() % (max - min + 1);
    for (i = 1; i < n; i++) {
        flag = 1;
        while(flag) {
            tmp = min + rand() % (max - min + 1);
            for (j = 0; j < i; j++) {
                if (rands[j] == tmp) break;
            }
            if (j == i) flag = 0;
        }
        rands[i] = tmp;
    }
}

struct point_t *load_data(IN const char *file_name, OUT int *points_count)
{
    const static int INCREASEMENT = 512;
    int buffer_size;
    FILE *fp;
    struct point_t *p, *old_p;
    fp = fopen(file_name, "r");
    if (fp == NULL) {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    *points_count = 0;
    buffer_size = INCREASEMENT;
    p = malloc(INCREASEMENT * sizeof(*p));
    CHECK_MALLOC(p);
    while (fscanf(fp, "%lf%lf", &(p[*points_count].x), &(p[*points_count].y)) == 2) {
        (*points_count)++;
        if (*points_count == buffer_size) {
            old_p = p;
            p = realloc(p,  (buffer_size += INCREASEMENT) * sizeof(*p));
        }
        if (p == NULL) {
            free(old_p);
            CHECK_MALLOC(p);
        }
    }
    fclose(fp);
    return p;
}

double get_distance(IN const struct point_t *a, IN const struct point_t *b)
{
    static double dx, dy;
    dx = a->x - b->x;
    dy = a->y - b->y;
    return sqrt(dx * dx + dy * dy);
}

void get_cluster_means(IN const struct cluster_t *cluster, int size, OUT struct point_t *means)
{
    int i;
    means->x = means->y = 0;
    for (i = 0; i < size; i++) {
        means->x += (cluster->points.at(&(cluster->points), i))->x;
        means->y += (cluster->points.at(&(cluster->points), i))->y;
    }
    means->x /= size;
    means->y /= size;
}

int get_point_cluster_id(IN const struct point_t *point, IN const struct point_t *means, int k)
{
    int i, id;
    double dist, tmp;
    id = 0;
    dist = get_distance(point, &means[0]);
    for (i = 1; i < k; i++) {
        tmp = get_distance(point, &means[i]);
        if (tmp < dist) {
            dist = tmp;
            id = i;
        }
    }
    return id;
}

double get_clusters_dispersion(IN const struct cluster_t *clusters, IN const struct point_t *means, int k)
{
    static double result;
    static int i, j;
    result = 0.0;
    for (i = 0; i < k; i++) {
        for (j = 0; j < clusters[i].points.size(&(clusters[i].points)); j++) {
            result += (clusters[i].points.at(&(clusters[i].points), j)->x - means[i].x) *
                      (clusters[i].points.at(&(clusters[i].points), j)->x - means[i].x) +
                      (clusters[i].points.at(&(clusters[i].points), j)->y - means[i].y) *
                      (clusters[i].points.at(&(clusters[i].points), j)->y - means[i].y);
        }
    }
    return result;
}

void k_means(IN const struct point_t *points, int points_count, int k, OUT struct point_t *means, OUT struct cluster_t *clusters)
{
    int i = 0;
    int id = -1;
    double old_dispersion, new_dispersion;
    int *rand_k = malloc(sizeof(int) * k);
    random_with_range_n(0, points_count - 1, k, rand_k);
    for (i = 0; i < k; i++) {
        means[i].x = points[rand_k[i]].x;
        means[i].y = points[rand_k[i]].y;
    }
    for (i = 0; i < points_count; i++) {
        id = get_point_cluster_id(&points[i], means, k);
        clusters[id].points.push_back(&(clusters[id].points), &points[i]);
    }
    old_dispersion = -1.0;
    new_dispersion = get_clusters_dispersion(clusters, means, k);
    while (fabs(new_dispersion - old_dispersion) >= DELTA) {
        for (i = 0; i < k; i++)
            get_cluster_means(&clusters[i],clusters[i].points.size(&(clusters[i].points)), &means[i]);
        old_dispersion = new_dispersion;
        new_dispersion = get_clusters_dispersion(clusters, means, k);
        for (i = 0; i < k; i++) clusters[i].points.clear(&(clusters[i].points));
        for (i = 0; i < points_count; i++) {
            id = get_point_cluster_id(&points[i], means, k);
            clusters[id].points.push_back(&(clusters[id].points), &points[i]);
        }
    }
    free(rand_k);
}

void write_clusters(const char *file_name, IN const struct cluster_t *clusters, int k)
{
    int i, j;
    FILE *fp;
    fp = fopen(file_name, "w+");
    if (fp == NULL) {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    for (i = 0; i < k; i++) {
        fprintf(fp, "%d:\n", i);
        for ( j = 0; j < clusters[i].points.size(&(clusters[i].points)); j++) {
            fprintf(fp, "%f %f\n", clusters[i].points.at(&(clusters[i].points), j)->x,
                                   clusters[i].points.at(&(clusters[i].points), j)->y);
        }
    }
    fclose(fp);
}


int main(int argc, char **argv)
{
    int i;
    int k, points_count;
    char *input_file_name, *output_file_name;
    struct point_t *points;
    struct point_t *means;
    struct cluster_t *clusters;

    input_file_name = argv[1];
    output_file_name = argv[2];
    k = atoi(argv[3]);
    points_count = 0;
    means = malloc(sizeof(*means) * k);
    CHECK_MALLOC(means);
    clusters = malloc(sizeof(*clusters) * k);
    CHECK_MALLOC(clusters);
    for (i = 0; i < k; i++)
        DL_VECTOR_INIT(point_p_vector, &(clusters[i].points));

    points = load_data(input_file_name, &points_count);
    k_means(points, points_count, k, means, clusters);
    write_clusters(output_file_name, clusters, k);

    for (i = 0; i < k; i++) {
        clusters[i].points.destroy(&(clusters[i].points));
    }
    free(clusters);
    free(means);
    free(points);    
    return 0;
}
