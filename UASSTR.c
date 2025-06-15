#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define MAX_LENGTH 50
#define MAX_CEK_AKUN 50
#define MAX_EXPLORASI 10

#define NAMA_WAHANA "Stellaris"

#define FILE_AKUN "data_akun.txt"
#define FILE_WAHANA "data_wahana.txt"
#define FILE_PESANAN "data_pesanan.txt"

#ifdef _WIN32
#define clear_terminal() system("cls")
#else
#define clear_terminal() system("clear")
#endif

// Data Login Admin
char nama_admin[] = "admin";
char pass_admin[] = "admin1234";

// Fungsi-fungsi kecil yang bisa dipakai
void hapusNewline(char *str)
{
    str[strcspn(str, "\n")] = 0;
}

void bersihkanBuffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
    {
    }
}

bool user_validasi_nama_akun(char *username)
{
    FILE *file = fopen(FILE_AKUN, "r");
    if (file == NULL)
    {
        return false;
    }

    char line[MAX_LENGTH * 2]; // Menampung akun + password

    while (fgets(line, sizeof(line), file))
    {
        hapusNewline(line);
        char username_terdaftar[MAX_CEK_AKUN];
        sscanf(line, "%[^,]", username_terdaftar);

        if (strcmp(username_terdaftar, username) == 0)
        {
            fclose(file);
            return true;
        }
    }
    fclose(file);
    return false;
}

// Kode ini untuk nambahin delay, alternatif dari delay() yang tidak bisa digunakan di ubuntu
void delay(int number_of_mili_seconds)
{
    // Menyimpan waktu mulai
    clock_t start_time = clock();

    // Melakukan perulangan sampai waktu yang dibutuhkan tercapai
    while (clock() < start_time + number_of_mili_seconds)
        ;
}

// ----- //

// Deklarasi dan inisialisasi struct
typedef struct pesanan
{
    char nama_akun[MAX_LENGTH];
    char nama_wahana[MAX_LENGTH];
    int kursi_dipesan;
    struct pesanan *next;
} pesanan;

typedef struct kumpulan_pesanan
{
    pesanan *first;
    pesanan *last;
    int total_pesanan;
} kumpulan_pesanan;

typedef struct wahana
{
    char nama[MAX_LENGTH];
    char tanggal[MAX_LENGTH]; // Format DD-MM-YY untuk ditampilin
    int tanggal_int;          // Untuk sorting tanggal
    int kursi;
    char deskripsi[300]; // Cukup 50 kata
    struct wahana *next;
} wahana;

typedef struct kumpulan_wahana
{
    wahana *first;
    wahana *last;
    int total_node;
} kumpulan_wahana;

void init_wahana(kumpulan_wahana *L)
{
    L->first = NULL;
    L->last = NULL;
    L->total_node = 0;
}

void init_pesanan(kumpulan_pesanan *L)
{
    L->first = NULL;
    L->last = NULL;
    L->total_pesanan = 0;
}

// ----- //

void sistem_load_wahana_dari_file(kumpulan_wahana *L, kumpulan_wahana *L_sorted);
void sistem_load_pesanan_dari_file(kumpulan_pesanan *L);

int admin_konversi_tanggalInt(const char *tanggal); // Admin untuk tambah wahana

void sistem_empty_sorted(kumpulan_wahana *L_sorted);
void admin_tambah_wahana(kumpulan_wahana *L_sorted);
void admin_hapus_wahana(kumpulan_wahana *L_sorted, char *hapus);
void admin_eksekusi_wahana(kumpulan_wahana *L_sorted, kumpulan_pesanan *p);
void sistem_hapus_pesanan_setelah_eksekusi(kumpulan_pesanan *L, char *nama_wahana);
void sistem_sync_to_file(kumpulan_wahana *L_sorted, kumpulan_pesanan *p, int pilihan); // 1 = wahana, 2 = pesanan
void ask_for_sync(kumpulan_wahana *L_sorted, kumpulan_pesanan *p)
{
    int ans;
    printf("\nApakah ingin melakukan sinkronisasi file sebelum keluar?\n[1 = Ya, 0 = Tidak]\n=== ");
    scanf("%i", &ans);
    if (ans == 1)
    {
        sistem_sync_to_file(L_sorted, NULL, 1);
    }
}
void user_tampilkan_pesanan(kumpulan_wahana *w_sorted, kumpulan_pesanan *L, char *nama);
void user_book_flight(kumpulan_wahana sorted, kumpulan_pesanan *L, kumpulan_pesanan *L_user, int ans, char *nama);
void user_cancel_flight(kumpulan_wahana *W_sorted, kumpulan_pesanan *L, kumpulan_pesanan *L_user, char *nama_user, char *nama_wahana);
void hapus_wahana_cancel_pesanan(int pilihan, kumpulan_pesanan *L, kumpulan_pesanan *L_user, kumpulan_wahana *W_sorted, char *hapus, char *nama);

bool admin_cek_wahana_dipesan(char *nama_wahana);
void admin_edit_flight(kumpulan_wahana *L_sorted);

void free_wahana_list(kumpulan_wahana *L);
void free_pesanan_list(kumpulan_pesanan *L);

// Bantuan untuk cek jawaban soal Kruskal dan Dijkstra
bool user_cek_jawaban_kruskal(int jawaban)
{
    // Bobot MST yang benar: edge dengan bobot 1 (A-C), 2 (B-C), 3 (C-D) = total 6
    return jawaban == 6;
}

bool user_cek_jawaban_dijkstra(int jawaban)
{
    // Jalur tercepat A->B->C->D: 1+2+1 = 4 menit
    return jawaban == 4;
}

// ----- //

// Sorting
void admin_execute_selection_sort(int total_node, kumpulan_wahana *unsorted, kumpulan_wahana *sorted);
void admin_transversal_add_wahana_baru(kumpulan_wahana *L_sorted, wahana *baru);

// Searching
void sistem_tampilkan_wahana_sorted(kumpulan_wahana *L_sorted, int role, bool *opsional);
void user_pisahkan_pesanan(kumpulan_pesanan *L, kumpulan_pesanan *L_user, char *nama);

// ----- //

void user_simulate_preparation();

int tampilan_whoareu()
{
    int ans = -1;
    while (ans > 3 || ans < 0)
    {
        clear_terminal();
        printf("--------------------------------------------");
        printf("\n\tSelamat Datang di %s!\n", NAMA_WAHANA);
        printf("--------------------------------------------\n");
        printf("\nAnda datang sebagai siapa?\n");
        printf("1. User\n2. Admin\n3. Keluar Program\n[1/2/3]\n");
        printf("\n=== ");
        scanf("%i", &ans);
        bersihkanBuffer();

        if (!(ans > 3 || ans < 0))
        {
            return ans;
        }
        else
        {
            printf("\n> Pilihan tidak valid\n");
        }
    }
}

int tampilan_after_whoareu()
{
    int ans = -1;
    while (ans < 0 || ans > 3)
    {
        clear_terminal();
        printf("--------------------------------------------");
        printf("\n\tSelamat Datang di %s!\n", NAMA_WAHANA);
        printf("--------------------------------------------\n");
        printf("\nPilih salah satu:\n");
        printf("1. Login\n2. Buat akun\n3. Kembali\n\n=== ");
        scanf("%i", &ans);
        bersihkanBuffer();

        if (!(ans > 3 || ans < 0))
        {
            return ans;
        }
        else
        {
            printf("> Pilihan tidak valid\n");
        }
    }
}

bool login_admin()
{
    char input_nama[MAX_LENGTH], input_password[MAX_LENGTH];

    clear_terminal();
    printf("--------------------------------------------");
    printf("\n\tSelamat Datang di %s!\n", NAMA_WAHANA);
    printf("--------------------------------------------\n");
    printf("\n\t===== FORM LOG IN =====\n\n");

    printf("Masukkan nama: ");
    fgets(input_nama, sizeof(input_nama), stdin);
    hapusNewline(input_nama);

    printf("Masukkan password: ");
    fgets(input_password, sizeof(input_password), stdin);
    hapusNewline(input_password);

    if (strcmp(nama_admin, input_nama) == 0 && strcmp(pass_admin, input_password) == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void login_user(int *login_status, char *nama_user_login)
{
    char input_nama[MAX_LENGTH], input_password[MAX_LENGTH];

    clear_terminal();
    printf("--------------------------------------------");
    printf("\n\tSelamat Datang di %s!\n", NAMA_WAHANA);
    printf("--------------------------------------------\n");
    printf("\n\t===== FORM LOG IN =====\n\n");

    printf("Masukkan nama: ");
    fgets(input_nama, sizeof(input_nama), stdin);
    hapusNewline(input_nama);

    printf("Masukkan password: ");
    fgets(input_password, sizeof(input_password), stdin);
    hapusNewline(input_password);

    FILE *file = fopen(FILE_AKUN, "r");
    if (!file)
        printf("\n> Tidak dapat membuka %s\n", FILE_AKUN);

    char line[MAX_CEK_AKUN * 2];
    while (fgets(line, sizeof(line), file))
    {
        char iterasi_nama_file[MAX_LENGTH], iterasi_password_file[MAX_LENGTH];
        sscanf(line, "%[^,],%s", iterasi_nama_file, iterasi_password_file);
        if (strcmp(iterasi_nama_file, input_nama) == 0 &&
            strcmp(iterasi_password_file, input_password) == 0)
        {
            fclose(file);
            strcpy(nama_user_login, input_nama);
            *login_status = 1;
            return;
        }
    }
    fclose(file);
    *login_status = 0;
    return;
}

void user_create_account()
{
    char input_nama[MAX_LENGTH], input_password[MAX_LENGTH];
    clear_terminal();
    printf("--------------------------------------------");
    printf("\n\tSelamat Datang di %s!\n", NAMA_WAHANA);
    printf("--------------------------------------------\n");
    printf("\n\t===== FORM PENDAFTARAN =====\n\n");

    while (true)
    {
        printf("Masukkan nama: ");
        fgets(input_nama, sizeof(input_nama), stdin);
        hapusNewline(input_nama);

        if (user_validasi_nama_akun(input_nama))
        {
            printf("\n> Nama sudah terdaftar. Coba masukan nama lain.\n\n");
            delay(500);
        }
        else
        {
            break;
        }
    }

    printf("Masukkan password: ");
    fgets(input_password, sizeof(input_password), stdin);
    hapusNewline(input_password);

    // Mendaftarkan akun ke data_akun.txt
    FILE *file = fopen(FILE_AKUN, "a");
    fprintf(file, "%s,%s\n", input_nama, input_password);
    fclose(file);

    printf("\n> Akun berhasil didaftarkan\n");
}

void dashboard_user(int *layer, kumpulan_wahana *w_sorted, kumpulan_pesanan *L, kumpulan_pesanan *L_user, char *nama_akun_user)
{
    int ans = 0;
    do
    {
        int book_status = 0;
        bool wahana_status = true;
        char target_wahana[MAX_LENGTH];
        clear_terminal();
        printf("--------------------------------------------");
        printf("\n\tSelamat Datang, %s.\n", nama_akun_user);
        printf("--------------------------------------------\n");
        printf("\nWahana penerbangan Anda yang akan datang :\n\n");

        user_tampilkan_pesanan(w_sorted, L_user, nama_akun_user);

        printf("\n1. Lihat Penerbangan Mendatang\n");
        printf("2. Batalkan Penerbangan\n");
        printf("3. Lakukan Simulasi Persiapan Penerbangan\n");
        printf("4. Logout\n");
        printf("5. Keluar\n\n=== ");
        scanf("%i", &ans);
        bersihkanBuffer();

        switch (ans)
        {
        case 1:
            clear_terminal();
            printf("--------------------------------------------");
            printf("\n\tSelamat Datang, %s.\n", nama_akun_user);
            printf("--------------------------------------------\n\n");
            sistem_tampilkan_wahana_sorted(w_sorted, 1, &wahana_status);
            if (wahana_status)
            {
                printf("---------------------------------------------\n");
                printf("\nApakah Anda ingin melakukan reservasi?\n[1 = Ya, 0 = Tidak]\n\n=== ");
                scanf("%d", &book_status);
                bersihkanBuffer();
                user_book_flight(*w_sorted, L, L_user, book_status, nama_akun_user);
            }
            else
            {
                printf("\n> Tidak ada wahana penerbangan dengan kursi tersisa\n");
                delay(500);
            }
            break;
        case 2:
            printf("\n---------------------------------------------\n");
            printf("\nMasukkan nama wahana yang ingin dibatalkan :\n=== ");
            fgets(target_wahana, sizeof(target_wahana), stdin);
            hapusNewline(target_wahana);
            hapus_wahana_cancel_pesanan(2, L, L_user, w_sorted, target_wahana, nama_akun_user);
            break;
        case 3:
            user_simulate_preparation();
            break;
        case 4:
            sistem_sync_to_file(NULL, L, 2);
            printf("\n> Menyinkronasi data...\n");
            *layer = 1;
            return;
        case 5:
            sistem_sync_to_file(NULL, L, 2);
            sistem_load_pesanan_dari_file(L);
            *layer = 4;
            return;
        default:
            printf("\n>  Tidak ada pada pilihan\n");
            delay(250);
            break;
        }
    } while (true);
}

void dashboard_admin(int *layer, kumpulan_wahana *L_sorted, kumpulan_pesanan *p)
{
    int ans = 0;
    char target_wahana[MAX_LENGTH];
    do
    {
        int hapus = -1;

        clear_terminal();
        printf("---------------------------------------");
        printf("\n\tSelamat Datang, Officer.\n");
        printf("---------------------------------------\n");
        printf("\nWahana Penerbangan Mendatang :\n\n");

        sistem_tampilkan_wahana_sorted(L_sorted, 2, NULL);

        printf("\n1. Tambah Wahana Penerbangan\n");
        printf("2. Hapus Wahana Penerbangan\n");
        printf("3. Edit Wahana Penerbangan\n");
        printf("4. Eksekusi Wahana Penerbangan\n");
        printf("5. Sinkronisasi Data Wahana Penerbangan\n");
        printf("6. Logout\n");
        printf("7. Keluar\n\n=== ");
        scanf("%i", &ans);
        bersihkanBuffer();

        switch (ans)
        {
        case 1:
            admin_tambah_wahana(L_sorted);
            break;
        case 2:
            printf("\n--------------------------------------------\n");
            printf("\nMasukkan nama wahana yang ingin dihapus.\n\n=== ");
            fgets(target_wahana, sizeof(target_wahana), stdin);
            hapusNewline(target_wahana);
            hapus_wahana_cancel_pesanan(1, NULL, NULL, L_sorted, target_wahana, NULL);
            break;
        case 3:
            admin_edit_flight(L_sorted);
            break;
        case 4:
            admin_eksekusi_wahana(L_sorted, p);
            printf("\n> Wahana explorasi berhasil diluncurkan\n");
            break;
        case 5:
            sistem_sync_to_file(L_sorted, NULL, 1);
            break;
        case 6:
            ask_for_sync(L_sorted, p);
            sistem_load_pesanan_dari_file(p);
            *layer = 1;
            return;
        case 7:
            ask_for_sync(L_sorted, p);
            *layer = 4;
            return;
        default:
            printf("\n> Tidak ada pada pilihan\n");
            delay(250);
            break;
        }
    } while (true);
}

// ----- //

// ## Program utama ##
int main()
{
    int layer = 1;
    kumpulan_wahana list_wahana;
    kumpulan_wahana list_wahana_sorted;

    kumpulan_pesanan list_pesanan;
    kumpulan_pesanan list_pesanan_user;
    char nama_user_login[MAX_LENGTH];

    init_wahana(&list_wahana);
    init_wahana(&list_wahana_sorted);
    sistem_load_wahana_dari_file(&list_wahana, &list_wahana_sorted);

    init_pesanan(&list_pesanan);
    sistem_load_pesanan_dari_file(&list_pesanan);
    while (1)
    {
        int user_or_admin = -1;
        // 1 = user, 2 = admin
        while (layer == 1)
        {
            // Tampilan untuk pilih masuk sebagai user atau admin
            user_or_admin = tampilan_whoareu();
            switch (user_or_admin)
            {
            case 1:
                layer++;
                break;
            case 2:
                layer++;
                break;
            case 3:
                return 0;
            }
        }
        while (layer == 2)
        {
            if (user_or_admin == 1)
            {                                        // Sebagai user
                int flow = tampilan_after_whoareu(); // Login, membuat akun, atau kembali
                int login_status_user = 0;
                switch (flow)
                {
                case 1:
                    login_user(&login_status_user, nama_user_login);

                    if (login_status_user == 1 && strlen(nama_user_login) > 0)
                    {
                        init_pesanan(&list_pesanan_user);
                        user_pisahkan_pesanan(&list_pesanan, &list_pesanan_user, nama_user_login);
                        layer++;
                    }
                    else
                    {
                        printf("\n> Login tidak valid\n");
                        delay(250);
                    }
                    break;
                case 2:
                    user_create_account();
                    break;
                case 3:
                    layer--;
                    break;
                default:
                    printf("\n> Pilihan tidak valid");
                    delay(250);
                    break;
                }
            }
            else if (user_or_admin == 2)
            { // Sebagai admin
                if (login_admin())
                {
                    sistem_load_pesanan_dari_file(&list_pesanan);
                    layer++;
                }
                else
                {
                    printf("\n> Login tidak valid\n");
                    delay(250);
                    layer--;
                }
            }
        }
        while (layer == 3)
        {
            int admin_answer;
            switch (user_or_admin)
            {
            case 1: // Sebagai user
                dashboard_user(&layer, &list_wahana_sorted, &list_pesanan, &list_pesanan_user, nama_user_login);
                break;
            case 2: // Sebagai admin
                dashboard_admin(&layer, &list_wahana_sorted, &list_pesanan);
                break;
            }
        }

        if (layer == 4)
        {
            free_wahana_list(&list_wahana);
            free_wahana_list(&list_wahana_sorted);
            free_pesanan_list(&list_pesanan);
            free_pesanan_list(&list_pesanan_user);

            return 0;
        }
    }
}

// ############

// ----- //

void sistem_load_wahana_dari_file(kumpulan_wahana *L, kumpulan_wahana *L_sorted)
{
    L->first = NULL;

    FILE *file = fopen(FILE_WAHANA, "r");

    // Case 1: Jika file tidak ada, kemudian membuat file baru
    if (!file)
    {
        printf("\n> File data_wahana.txt tidak ditemukan. Membuat file baru.\n");
        file = fopen(FILE_WAHANA, "w");
        if (file)
        {
            fclose(file);
        }
        else
        {
            printf("\n> Gagal membuat file data_wahana.txt\n");
        }
        return;
    }

    // Case 2: Jika file ada tapi kosong
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    if (file_size == 0)
    {
        printf("\n> File data_wahana.txt kosong. Linked list dibiarkan kosong.\n");
        fclose(file);
        return;
    }
    rewind(file);

    // Case 3: Jika file dan isinya ada
    char baris[500];
    int data_count = 0;

    while (fgets(baris, sizeof(baris), file))
    {
        // Skip baris kosong
        if (strlen(baris) <= 1)
            continue;

        wahana *baru = (wahana *)malloc(sizeof(wahana));
        if (sscanf(baris, "%[^,],%[^,],%d,%d,%[^\n]",
                   baru->nama, baru->tanggal, &baru->tanggal_int, &baru->kursi, baru->deskripsi) == 5)
        {
            baru->next = L->first;
            L->first = baru;
            if (L->first == NULL)
            {
                L->first = L->last = baru;
            }
            else
            {
                L->last->next = baru;
                L->last = baru;
            }
            baru->next = NULL;
            L->total_node++;
            data_count++;
        }
        else
        {
            free(baru);
        }
    }

    fclose(file);

    if (data_count > 0)
    {
        printf("\n> Berhasil memuat %d data wahana dari file.\n", data_count);
    }
    // Langsung melakukan proses sorting
    admin_execute_selection_sort(data_count, L, L_sorted);
}

int admin_konversi_tanggalInt(const char *tanggal)
{
    int dd, mm, yy;
    sscanf(tanggal, "%d-%d-%d", &dd, &mm, &yy);
    yy += (yy < 50) ? 2000 : 1900;
    return yy * 10000 + mm * 100 + dd;
}

void admin_tambah_wahana(kumpulan_wahana *L_sorted)
{
    clear_terminal();
    printf("--------------------------------------------");
    printf("\n\t===== Form Tambah Wahana =====\n");
    printf("--------------------------------------------\n");
    wahana *baru = malloc(sizeof(wahana));
    if (!baru)
    {
        printf("\n> Gagal mengalokasikan memori!\n");
        return;
    }

    // Input nama
    printf("\nMasukkan nama wahana: ");
    if (!fgets(baru->nama, sizeof(baru->nama), stdin))
    {
        printf("\n> Gagal membaca input nama\n");
        free(baru);
        delay(250);
        return;
    }
    hapusNewline(baru->nama);

    // Input tanggal
    printf("Masukkan tanggal peluncuran (DD-MM-YY): ");
    if (!fgets(baru->tanggal, sizeof(baru->tanggal), stdin))
    {
        printf("\n> Gagal membaca input tanggal\n");
        free(baru);
        delay(250);
        return;
    }
    hapusNewline(baru->tanggal);
    baru->tanggal_int = admin_konversi_tanggalInt(baru->tanggal);

    // Input kursi
    printf("Masukkan jumlah kursi: ");
    if (scanf("%d", &baru->kursi) != 1 || baru->kursi <= 0)
    {
        printf("\n> Input kursi tidak valid! Angka harus positif\n");
        bersihkanBuffer();
        free(baru);
        delay(250);
        return;
    }
    bersihkanBuffer();

    // Input deskripsi
    printf("Masukkan deskripsi (maks 50 kata): ");
    if (!fgets(baru->deskripsi, sizeof(baru->deskripsi), stdin))
    {
        printf("\n> Gagal membaca input deskripsi\n");
        free(baru);
        delay(250);
        return;
    }
    hapusNewline(baru->deskripsi);

    // Tambahkan ke linked list
    admin_transversal_add_wahana_baru(L_sorted, baru);

    // Simpan ke file
    FILE *file = fopen(FILE_WAHANA, "a");
    if (!file)
    {
        printf("\n> Gagal membuka file %s\n", FILE_WAHANA);
        free(baru);
        return;
    }
    fprintf(file, "%s,%s,%d,%d,%s\n",
            baru->nama, baru->tanggal,
            baru->tanggal_int, baru->kursi,
            baru->deskripsi);
    fclose(file);

    printf("\n> Wahana berhasil ditambahkan!\n");
    delay(500);
}

void admin_eksekusi_wahana(kumpulan_wahana *L_sorted, kumpulan_pesanan *p)
{
    if (L_sorted->first == NULL)
    {
        printf("\n> Tidak ada wahana yang tercatat\n");
        delay(250);
        return;
    }

    wahana *hapus = L_sorted->first;
    char target_wahana[MAX_LENGTH];
    strcpy(target_wahana, hapus->nama);

    L_sorted->first = hapus->next;
    if (L_sorted->first == NULL)
        L_sorted->last = NULL;
    free(hapus);
    L_sorted->total_node--;

    sistem_hapus_pesanan_setelah_eksekusi(p, target_wahana);
    sistem_sync_to_file(L_sorted, p, 1);
    sistem_sync_to_file(L_sorted, p, 2);
}

void sistem_hapus_pesanan_setelah_eksekusi(kumpulan_pesanan *L, char *nama_wahana)
{
    pesanan *current = L->first;
    pesanan *prev = NULL;

    while (current != NULL)
    {
        if (strcmp(current->nama_wahana, nama_wahana) == 0)
        {
            pesanan *hapus = current;

            if (prev == NULL)
            {
                L->first = current->next;
            }
            else
            {
                prev->next = current->next;
            }

            current = current->next;
            free(hapus);
            continue;
        }
        prev = current;
        current = current->next;
    }
}

void admin_hapus_wahana(kumpulan_wahana *L_sorted, char *hapus)
{
    wahana *prev = NULL;
    wahana *hook = L_sorted->first;

    if (admin_cek_wahana_dipesan(hapus))
    {
        printf("\n> Wahana ini sudah dipesan, regulasi membatasi wewenang admin untuk melakukan penghapusan.\n");
        delay(500);
        return;
    }

    while (hook != NULL)
    {
        if (strcmp(hook->nama, hapus) == 0)
        {
            if (prev == NULL)
            {
                prev = L_sorted->first;
                L_sorted->first = L_sorted->first->next;
                free(prev);
                return;
            }
            else
            {
                if (hook == L_sorted->last)
                {
                    L_sorted->last = prev;
                    free(hook);
                    return;
                }
                else
                {
                    prev->next = hook->next;
                    free(hook);
                }
            }
        }
        prev = hook;
        hook = hook->next;
    }
}

// ----- //

// Selection sorting
void admin_execute_selection_sort(int total_node, kumpulan_wahana *unsorted, kumpulan_wahana *sorted)
{
    wahana *current;

    sistem_empty_sorted(sorted);
    sorted->first = NULL;
    wahana *tail_sorted = NULL;

    for (int i = 0; i < total_node; i++)
    {

        current = unsorted->first;
        wahana *min = unsorted->first;
        wahana *prev_current = NULL;
        wahana *prev_min = NULL;

        // Mencari node min
        while (current != NULL)
        {
            if (current->tanggal_int < min->tanggal_int)
            {
                min = current;
                prev_min = prev_current;
            }

            prev_current = current;
            current = current->next;
        }

        // Memisahkan min dari unsorted
        if (prev_min == NULL)
            unsorted->first = min->next;
        else
            prev_min->next = min->next;

        // Masukkan min ke sorted
        if (sorted->first == NULL)
        {
            sorted->first = min;
            tail_sorted = min;
        }
        else
        {
            tail_sorted->next = min;
            tail_sorted = min;
        }
        min->next = NULL;
    }
}

// ----- //

void admin_transversal_add_wahana_baru(kumpulan_wahana *L_sorted, wahana *baru)
{
    if (!baru || !L_sorted)
    {
        printf("\n> Parameter tidak valid\n");
        return;
    }

    baru->next = NULL;

    // Jika list kosong
    if (!L_sorted->first)
    {
        L_sorted->first = L_sorted->last = baru;
        L_sorted->total_node++;
        return;
    }

    // Cari posisi yang pas untuk insert
    wahana *current = L_sorted->first;
    wahana *prev = NULL;

    while (current && baru->tanggal_int >= current->tanggal_int)
    {
        prev = current;
        current = current->next;
    }

    // Insert di awal
    if (!prev)
    {
        baru->next = L_sorted->first;
        L_sorted->first = baru;
    }
    // Insert di tengah atau akhir
    else
    {
        prev->next = baru;
        baru->next = current;

        // Jika insert di akhir
        if (!current)
        {
            L_sorted->last = baru;
        }
    }

    L_sorted->total_node++;
}
// L_sorted hanya ditampilkan di dashboard admin, dan saat user book flight
void sistem_tampilkan_wahana_sorted(kumpulan_wahana *L_sorted, int role, bool *opsional)
{
    if (L_sorted->first == NULL)
    {
        printf("\n> Tidak ada wahana penerbangan yang ditemukan\n\n");
        if (opsional != NULL)
            *opsional = false;
        return;
    }

    int count = 0;
    wahana *current = L_sorted->first;
    if (role == 2)
    {
        while (current != NULL)
        {
            printf("%d. %s\n", ++count, current->nama);
            printf("Deskripsi = %s\n", current->deskripsi);
            printf("Tanggal peluncuran = %s\n", current->tanggal);
            printf("Kapasitas kursi tersisa = %i\n\n", current->kursi);
            current = current->next;
        }
    }
    else if (role == 1)
    {
        if (current->kursi <= 0)
        {
            *opsional = false;
            return;
        }
        while (current != NULL && current->kursi > 0)
        {
            printf("%d. %s\n", ++count, current->nama);
            printf("Deskripsi = %s\n", current->deskripsi);
            printf("Tanggal peluncuran = %s\n", current->tanggal);
            printf("Kapasitas kursi tersisa = %i\n\n", current->kursi);
            current = current->next;
        }
    }
}

// Refresh linked list wahana sorted
void sistem_empty_sorted(kumpulan_wahana *L_sorted)
{
    wahana *current = L_sorted->first, *temp;
    while (current != NULL)
    {
        temp = current;
        current = current->next;
        free(temp);
    }
    L_sorted->first = NULL;
    L_sorted->last = NULL;
    L_sorted->total_node = 0;
}

// Tulis ulang seluruh file
void sistem_sync_to_file(kumpulan_wahana *L_sorted, kumpulan_pesanan *p, int pilihan)
{
    FILE *file = NULL;

    switch (pilihan)
    {
    case 1:
        file = fopen(FILE_WAHANA, "w");
        if (!file)
        {
            printf("\n> Gagal membuka file %s untuk disinkronisasi\n", FILE_WAHANA);
            return;
        }

        if (L_sorted == NULL || L_sorted->first == NULL)
            return;

        wahana *current = L_sorted->first;
        while (current != NULL)
        {
            fprintf(file, "%s,%s,%d,%d,%s\n", current->nama, current->tanggal, current->tanggal_int, current->kursi, current->deskripsi);
            current = current->next;
        }
        fclose(file);
        printf("\n> Berhasil melakukan sinkronasi file\n");
        delay(250);
        break;
    case 2:
        file = fopen(FILE_PESANAN, "w");
        if (!file)
        {
            printf("\n> Gagal membuka file %s untuk disinkronisasi\n", FILE_PESANAN);
            return;
        }

        if (p == NULL || p->first == NULL)
            return;

        pesanan *current_pesanan = p->first;
        while (current_pesanan)
        {
            fprintf(file, "%s,%s,%d\n", current_pesanan->nama_akun, current_pesanan->nama_wahana, current_pesanan->kursi_dipesan);
            current_pesanan = current_pesanan->next;
        }
        fclose(file);
        printf("\n> Data pesanan berhasil disinkronisasi\n");
        delay(250);
        break;
    default:
        printf("\n> Pilihan tidak tersedia\n");
        delay(250);
        break;
    }
}

void sistem_load_pesanan_dari_file(kumpulan_pesanan *L)
{
    // Inisialisasi ulang linked list
    if (L && L->first)
    {
        free_pesanan_list(L); // Semua node lama dibebaskan
    }
    L->first = L->last = NULL;
    L->total_pesanan = 0;

    FILE *file = fopen(FILE_PESANAN, "r");

    // Kalau file tidak ada, buat baru
    if (!file)
    {
        printf("\n> File %s tidak ditemukan. Membuat file baru.\n", FILE_PESANAN);
        file = fopen(FILE_PESANAN, "w");
        if (file)
            fclose(file);
        else
            printf("\n> Gagal membuat file %s\n", FILE_PESANAN);
        return;
    }

    char baris[500];
    int data_count = 0;

    while (fgets(baris, sizeof(baris), file))
    {
        // Skip baris kosong
        if (strlen(baris) <= 1)
            continue;

        pesanan *baru = malloc(sizeof(pesanan));
        if (!baru)
        {
            printf("\n> Gagal mengalokasikan memori untuk node pesanan\n");
            continue;
        }

        if (sscanf(baris, "%49[^,],%49[^,],%d",
                   baru->nama_akun,
                   baru->nama_wahana,
                   &baru->kursi_dipesan) != 3)
        {
            printf("\n> Gagal membaca baris: %s", baris);
            free(baru);
            continue;
        }

        baru->next = NULL;

        // Tambahkan ke linked list
        if (!L->first)
        {
            L->first = L->last = baru;
        }
        else
        {
            L->last->next = baru;
            L->last = baru;
        }
        baru->next = NULL;
        L->total_pesanan++;
        data_count++;
    }

    fclose(file);

    printf("\n> Berhasil memuat %i pesanan dari file %s\n", data_count, FILE_PESANAN);
}

// ----- //

void user_pisahkan_pesanan(kumpulan_pesanan *L, kumpulan_pesanan *L_user, char *nama)
{
    pesanan *current = L->first;
    pesanan *current_user = L_user->first;

    while (current != NULL)
    {
        if (strcmp(current->nama_akun, nama) == 0)
        {
            pesanan *wadah = (pesanan *)malloc(sizeof(pesanan));
            *wadah = *current;
            wadah->next = NULL;

            if (L_user->first == NULL)
            {
                L_user->first = wadah;
                L_user->last = wadah;
            }
            else
            {
                L_user->last->next = wadah;
                L_user->last = wadah;
            }
        }
        current = current->next;
    }
}

void user_tampilkan_pesanan(kumpulan_wahana *w_sorted, kumpulan_pesanan *L_user, char *nama)
{
    if (L_user->first == NULL)
    {
        printf("> User %s belum book flight\n", nama);
        return;
    }
    else
    {
        int count = 0;
        pesanan *current_pesanan = L_user->first;
        while (current_pesanan != NULL)
        {
            wahana *current_wahana = w_sorted->first;
            while (current_wahana != NULL)
            {
                if (strcmp(current_wahana->nama, current_pesanan->nama_wahana) == 0)
                {
                    printf("%d. %s\n", ++count, current_wahana->nama);
                    printf("Deskripsi = %s\n", current_wahana->deskripsi);
                    printf("Tanggal peluncuran = %s\n", current_wahana->tanggal);
                    printf("Jumlah kursi dipesan = %i\n\n", current_pesanan->kursi_dipesan);
                }
                current_wahana = current_wahana->next;
            }
            current_pesanan = current_pesanan->next;
        }
    }
}

void user_book_flight(kumpulan_wahana sorted, kumpulan_pesanan *L, kumpulan_pesanan *L_user, int ans, char *nama)
{
    if (ans != 1)
        return;
    char target_wahana[MAX_LENGTH];
    int kursi_dipesan;

    wahana *current = sorted.first;
    pesanan *hook = L->first;

    printf("\n---------------------------------------------\n");
    printf("\nMasukkan nama wahana yang ingin dipesan :\n=== ");
    fgets(target_wahana, sizeof(target_wahana), stdin);
    hapusNewline(target_wahana);

    while (current != NULL)
    {
        if (strcmp(current->nama, target_wahana) == 0)
        {
            printf("\nMasukkan jumlah kursi yang ingin dipesan :\n=== ");
            scanf("%d", &kursi_dipesan);
            bersihkanBuffer();

            if (kursi_dipesan > current->kursi || kursi_dipesan <= 0)
            {
                printf("\n> Jumlah kursi tidak valid. Tersedia: %d kursi.\n", current->kursi);
                delay(350);
                return;
            }

            current->kursi -= kursi_dipesan;

            pesanan *p_user = (pesanan *)malloc(sizeof(pesanan));

            strcpy(p_user->nama_akun, nama);
            strcpy(p_user->nama_wahana, target_wahana);
            p_user->kursi_dipesan = kursi_dipesan;
            p_user->next = NULL;

            // Simpan di linked list user
            if (!L_user->first)
            {
                L_user->first = p_user;
                L_user->last = p_user;
            }
            else
            {
                L_user->last->next = p_user;
                L_user->last = p_user;
            }

            // Simpan di linked list pesanan utama
            pesanan *p_global = (pesanan *)malloc(sizeof(pesanan));

            strcpy(p_global->nama_akun, p_user->nama_akun);
            strcpy(p_global->nama_wahana, p_user->nama_wahana);
            p_global->kursi_dipesan = p_user->kursi_dipesan;
            p_global->next = NULL;

            if (L->first == NULL)
            {
                L->first = p_global;
                L->last = p_global;
            }
            else
            {
                L->last->next = p_global;
                L->last = p_global;
            }

            L->total_pesanan++;

            // Simpan di file
            FILE *file = fopen(FILE_PESANAN, "a");
            fprintf(file, "%s,%s,%d\n", nama, current->nama, kursi_dipesan);
            fclose(file);
            printf("\n> Berhasil menambahkan pesanan di file %s\n", FILE_PESANAN);
            return;
        }
        current = current->next;
    }
    printf("\n> Wahana tidak ditemukan\n");
    delay(250);
}

void user_cancel_flight(kumpulan_wahana *W_sorted, kumpulan_pesanan *L, kumpulan_pesanan *L_user, char *nama_user, char *nama_wahana)
{
    pesanan *current_user = L_user->first;
    pesanan *prev_user = NULL;
    int kursi_dipesan = 0;
    bool ditemukan = false;

    // Hapus dari L_user
    while (current_user != NULL)
    {
        if (strcmp(current_user->nama_wahana, nama_wahana) == 0 && strcmp(current_user->nama_akun, nama_user) == 0)
        {
            kursi_dipesan = current_user->kursi_dipesan;
            ditemukan = true;

            if (prev_user == NULL)
            {
                L_user->first = current_user->next;
            }
            else
            {
                prev_user->next = current_user->next;
            }

            free(current_user);
            L_user->total_pesanan--;
            break;
        }
        prev_user = current_user;
        current_user = current_user->next;
    }

    if (!ditemukan)
    {
        printf("\n> Tidak ditemukan pesanan dengan nama wahana tersebut\n");
        delay(250);
        return;
    }

    // Hapus dari L (semua pesanan)
    pesanan *current_pesanan = L->first;
    pesanan *prev = NULL;

    while (current_pesanan != NULL)
    {
        if (strcmp(current_pesanan->nama_wahana, nama_wahana) == 0 && strcmp(current_pesanan->nama_akun, nama_user) == 0)
        {
            if (prev == NULL)
            {
                L->first = current_pesanan->next;
            }
            else
            {
                prev->next = current_pesanan->next;
            }
            free(current_pesanan);
            L->total_pesanan--;
            break;
        }
        prev = current_pesanan;
        current_pesanan = current_pesanan->next;
    }

    // Update kursi ke wahana
    wahana *current_wahana = W_sorted->first;
    while (current_wahana != NULL)
    {
        if (strcmp(current_wahana->nama, nama_wahana) == 0)
        {
            current_wahana->kursi += kursi_dipesan;
            break;
        }
        current_wahana = current_wahana->next;
    }

    printf("\n> Pesanan berhasil dibatalkan.\n");
    delay(500);
}

// ----- //

void hapus_wahana_cancel_pesanan(int pilihan, kumpulan_pesanan *L, kumpulan_pesanan *L_user, kumpulan_wahana *W_sorted, char *hapus, char *nama)
{
    if (pilihan == 1)
    {
        admin_hapus_wahana(W_sorted, hapus);
    }
    else if (pilihan == 2)
    {
        user_cancel_flight(W_sorted, L, L_user, nama, hapus);
    }
    else
    {
        printf("\n> Tidak ada di pilihan\n");
        delay(250);
    }
    return;
}

// ----- //

bool admin_cek_wahana_dipesan(char *nama_wahana)
{
    FILE *file = fopen(FILE_PESANAN, "r");
    if (!file)
    {
        return false;
    }
    else
    {
        printf("\n> Berhasil membuka file %s\n", FILE_PESANAN);
    }

    char baris[150];
    char akun_pemesan[MAX_LENGTH], wahana_dipesan[MAX_LENGTH];
    int kursi;

    while (fgets(baris, sizeof(baris), file))
    {
        if (sscanf(baris, "%[^,],%[^,],%d", akun_pemesan, wahana_dipesan, &kursi) == 3)
        {
            if (strcmp(nama_wahana, wahana_dipesan) == 0)
            {
                fclose(file);
                return true;
            }
        }
    }
    fclose(file);
    return false;
}

void admin_edit_flight(kumpulan_wahana *L_sorted)
{
    char target_wahana[MAX_LENGTH];
    printf("\n---------------------------------------------\n");
    printf("\nMasukkan nama wahana yang ingin diedit :\n=== ");
    fgets(target_wahana, sizeof(target_wahana), stdin);
    hapusNewline(target_wahana);

    if (admin_cek_wahana_dipesan(target_wahana))
    {
        printf("\n> Wahana ini sudah dipesan, regulasi membatasi wewenang admin untuk melakukan edit.\n");
        delay(350);
        return;
    }

    wahana *current = L_sorted->first;
    while (current != NULL)
    {
        if (strcmp(current->nama, target_wahana) == 0)
        {
            printf("\n--------------------------------------------\n\n");
            printf("Edit Data untuk Wahana %s\n\n", current->nama);

            printf("Tanggal peluncuran (DD-MM-YY): ");
            fgets(current->tanggal, sizeof(current->tanggal), stdin);
            hapusNewline(current->tanggal);
            current->tanggal_int = admin_konversi_tanggalInt(current->tanggal);

            printf("Jumlah kursi baru: ");
            scanf("%d", &current->kursi);
            bersihkanBuffer();

            printf("Deskripsi baru: ");
            fgets(current->deskripsi, sizeof(current->deskripsi), stdin);
            hapusNewline(current->deskripsi);

            printf("\nWahana berhasil diedit.\n");
            delay(250);
            return;
        }
        current = current->next;
    }
    printf("\n> Wahana tidak ditemukan\n");
    delay(250);
}

// ----- //

void free_wahana_list(kumpulan_wahana *L)
{
    if (L == NULL || L->first == NULL)
        return;
    wahana *current = L->first, *tmp;
    while (current != NULL)
    {
        tmp = current->next;
        free(current);
        current = tmp;
    }
    L->first = L->last = NULL;
    L->total_node = 0;
}

void free_pesanan_list(kumpulan_pesanan *L)
{
    if (L == NULL || L->first == NULL)
        return;
    pesanan *current = L->first, *tmp;
    while (current)
    {
        tmp = current->next;
        free(current);
        current = tmp;
    }
    L->first = L->last = NULL;
    L->total_pesanan = 0;
}

// ----- //

void user_simulate_preparation()
{
    int choice = 0;
    while (choice != 3)
    {
        clear_terminal();
        printf("------------------------------------------------------------");
        printf("\n\t===== MODUL SIMULASI LATIHAN ASTRONOT =====\n");
        printf("------------------------------------------------------------\n");
        printf("\nSelamat datang, calon astronot!\nHari ini Anda akan mengikuti dua latihan penting\n");
        printf("untuk mengasah kemampuan navigasi dan pengoptimalan sistem\ndalam misi luar angkasa.\n");
        printf("\nPilih tantangan yang ingin Anda kerjakan:\n\n");

        printf("1. Optimasi Sistem Kapal [Algoritma Kruskal]\n");
        printf("2. Mencari Jalur Tercepat [Algoritma Dijkstra]\n");
        printf("3. Kembali ke Dashboard\n");
        printf("\n=== ");
        scanf("%d", &choice);
        bersihkanBuffer();

        switch (choice)
        {
        case 1:
        {
            clear_terminal();
            printf("---------------------------------------------------------------------------");
            printf("\n\t--- Tantangan 1: Optimasi sistem kapal luar angkasa ---\n");
            printf("---------------------------------------------------------------------------\n");
            printf("\nDetail misi: Kapal Anda memiliki beberapa modul yang saling terhubung.\n");
            printf("Untuk menghemat daya dan meminimalkan kabel penghubung, Anda harus mencari\n");
            printf("penghubung minimal yang tetap menghubungkan semua modul secara efisien.\n");
            printf("Terapkan algoritma Kruskal untuk menentukan Minimum Spanning Tree (MST).\n\n");

            printf("Visualisasi modul dan koneksi kapal:\n\n");
            printf("      [A]\n");
            printf("     /   \\\n");
            printf("   4/     \\1\n");
            printf("   /       \\\n");
            printf(" [B]---2---[C]\n");
            printf("   \\       /\n");
            printf("    5\\   /3\n");
            printf("      [D]\n\n");

            printf("Daftar koneksi (arah tidak berpengaruh):\n");
            printf("1) A - B : 4 unit kabel\n");
            printf("2) A - C : 1 unit kabel\n");
            printf("3) B - C : 2 unit kabel\n");
            printf("4) B - D : 5 unit kabel\n");
            printf("5) C - D : 3 unit kabel\n\n");

            printf("Instruksi: Masukkan total jumlah unit kabel paling sedikit\n");
            printf("yang diperlukan untuk menghubungkan semua modul tanpa membentuk siklus.\n");

            int jawaban;
            printf("\nJawaban Anda : ");
            int cek = scanf("%d", &jawaban);
            bersihkanBuffer();
            if (cek != 1)
            {
                printf("\n> Input tidak valid. Masukkan angka bulat.\n");
            }
            else if (user_cek_jawaban_kruskal(jawaban))
            {
                printf("\nJawaban benar! Total kabel minimal adalah %d unit.\n", jawaban);
            }
            else
            {
                printf("\nJawaban salah. Jawaban yang benar adalah 6 unit kabel.\n");
            }
            printf("\nTekan Enter untuk kembali ke menu latihan...");
            getchar();
            break;
        }
        case 2:
        {
            clear_terminal();
            printf("-------------------------------------------------------------------------------");
            printf("\n\t--- Tantangan 2: Menentukan jalur tercepat ke modul tujuan ---\n");
            printf("-------------------------------------------------------------------------------\n");
            printf("\nDetail misi: Kendali misi ingin mengetahui jalur tercepat bagi shuttle suplai\n");
            printf("yang berangkat dari Modul A menuju Modul D.\n");
            printf("Berbagai rute memiliki waktu tempuh berbeda (dinyatakan dalam menit).\n");
            printf("Gunakan algoritma Dijkstra untuk mencari jalur tercepat.\n\n");

            printf("Visualisasi modul stasiun dan waktu tempuh:\n\n");
            printf("      [A]\n");
            printf("     /   \\\n");
            printf("   1/     \\4\n");
            printf("   /       \\\n");
            printf(" [B]---2---[C]\n");
            printf("   \\       /\n");
            printf("    5\\   /1\n");
            printf("      [D]\n\n");

            printf("Daftar rute:\n");
            printf("1) A -> B : 1 menit\n");
            printf("2) A -> C : 4 menit\n");
            printf("3) B -> C : 2 menit\n");
            printf("4) B -> D : 5 menit\n");
            printf("5) C -> D : 1 menit\n\n");

            printf("Instruksi: Masukkan total waktu tercepat dari Modul A ke Modul D.\n");

            int jawaban;
            printf("\nJawaban Anda (dalam menit) : ");
            int cek = scanf("%d", &jawaban);
            bersihkanBuffer();
            if (cek != 1)
            {
                printf("\n> Input tidak valid. Masukkan angka bulat.\n");
            }
            else if (user_cek_jawaban_dijkstra(jawaban))
            {
                printf("\nJawaban benar! Jalur tercepat memerlukan total waktu %d menit.\n", jawaban);
            }
            else
            {
                printf("\nJawaban salah. Jawaban yang benar adalah 4 menit.\n");
            }
            printf("\nTekan Enter untuk kembali ke menu latihan...");
            getchar();
            break;
        }
        case 3:
            printf("\nKembali ke dashboard.\n");
            break;

        default:
            printf("\n> Pilihan tidak valid. Silakan pilih 1, 2, atau 3.\n");
            break;
        }
    }
}
