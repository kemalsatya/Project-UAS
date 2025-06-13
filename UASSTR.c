#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LENGTH 50
#define MAX_CEK_AKUN 50
#define MAX_EXPLORASI 10

#define FILE_AKUN "data_akun.txt"
#define FILE_WAHANA "data_wahana.txt"
#define FILE_PESANAN "data_pesanan.txt"

// Log in untuk admin ///////////////////////////////////////////////////
char nama_admin[] = "admin";
char pass_admin[] = "admin1234";

// Beberapa fungsi kecil yang bisa dipakai ///////////////////////////////////
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

    char line[MAX_LENGTH * 2]; // menampung akun + password

    while (fgets(line, sizeof(line), file))
    {
        hapusNewline(line);
        char username_terdaftar[MAX_CEK_AKUN];
        sscanf(line, "%[^,]", username_terdaftar);

        if (strcmp(username_terdaftar, username) == 0)
        {
            fclose(file); //
            return true;
        }
    }
    fclose(file);
    return false;
}

////////////////////////////////////////////////////////

/*
## sistem penyimpanan internal untuk :
    1. Data register, login user dan log in admin -> list_akun.txt
    2. Data wahana dan pemesannya -> pesanan.txt
    3. Data wahana -> data_wahana.txt

    isi file :
    1. list_akun.txt -> Nama : [nama_user] | Password : [password_user]
    2. pesanan.txt -> Nama: Rani | Wahana: Casio moon rocket
    3. data_wahana.txt -> judul, deskripsi, tanggal keberangkatan, kursi.

    isi file data_wahana.txt sama pesanan.txt bakal diambil buat ditaroh di linked list, biar bisa ditampilin
    data_wahana.txt => ditambah, dihapus, dicari data kursi yang kosong
    pesanan.txt => ditampilin pesanan kursi user, hapus pesanan user
##
*/

//// deklarasi dan inisialisasi struct /////////////////////////////////////////////////////////////////

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
    char tanggal[MAX_LENGTH]; // format DD-MM-YY untuk ditampilin
    int tanggal_int;          // untuk sorting
    int kursi;
    char deskripsi[300]; // cukup 50 kata
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
    printf("\ninit berhasil\n");
}

////////////////////////////////////////////////////////////////////////////

void sistem_load_wahana_dari_file(kumpulan_wahana *L, kumpulan_wahana *L_sorted);
void sistem_load_pesanan_dari_file(kumpulan_pesanan *L);

int admin_konversi_tanggalInt(const char *tanggal); // oleh admin untuk tambah wahana

void sistem_empty_sorted(kumpulan_wahana *L_sorted);
void admin_tambah_wahana(kumpulan_wahana *L_sorted);
void admin_hapus_wahana(kumpulan_wahana *L_sorted, int hapus);
void admin_eksekusi_wahana(kumpulan_wahana *L_sorted, kumpulan_pesanan *p);
void sistem_hapus_pesanan_setelah_eksekusi(kumpulan_pesanan *L, char *nama_wahana);
void sistem_sync_to_file(kumpulan_wahana *L_sorted, kumpulan_pesanan *p, int pilihan); // 1 = wahana, 2 = pesanan
void ask_for_sync(kumpulan_wahana *L_sorted, kumpulan_pesanan *p)
{
    int ans;
    printf("\nDo you want to sync file before leave ?\n[1 = yes]\n== ");
    scanf("%i", &ans);
    if (ans == 1)
    {
        sistem_sync_to_file(L_sorted, NULL, 1);
    }
}
void user_tampilkan_pesanan(kumpulan_wahana *w_sorted, kumpulan_pesanan *L, char *nama);
void user_book_flight(kumpulan_wahana sorted, kumpulan_pesanan *L, kumpulan_pesanan *L_user, int ans, char *nama);
void user_cancel_flight(kumpulan_pesanan *L, kumpulan_pesanan *L_user, char *nama_user, int hapus);
void hapus_wahana_cancel_pesanan(int pilihan, kumpulan_pesanan *L, kumpulan_pesanan *L_user, kumpulan_wahana *W_sorted, int hapus, char *nama);

bool admin_cek_wahana_dipesan(char *nama_wahana);
void admin_edit_flight(kumpulan_wahana *L_sorted);

void free_wahana_list(kumpulan_wahana *L);
void free_pesanan_list(kumpulan_pesanan *L);

////// Helper untuk cek jawaban soal Kruskal dan Dijkstra ///////////////////////////////////////////////////////

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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

// sorting  //////////
void admin_execute_selection_sort(int total_node, kumpulan_wahana *unsorted, kumpulan_wahana *sorted);
void admin_transversal_add_wahana_baru(kumpulan_wahana *L_sorted, wahana *baru);
////////////////////////////////

// searching ///////
void sistem_tampilkan_wahana_sorted(kumpulan_wahana *L_sorted, int role);
void user_pisahkan_pesanan(kumpulan_pesanan *L, kumpulan_pesanan *L_user, char *nama);
////////////////////

void user_simulate_preparation();

int tampilan_whoareu()
{
    int ans = -1;
    while (ans > 3 || ans < 0)
    {
        printf("\nWelcome to Antacha\n");
        printf("Who are you ?\n");
        printf("1. User\n2. Admin\n3. Exit program\n[1/2/3]\n");
        printf("\n== ");
        scanf("%i", &ans);
        bersihkanBuffer();

        if (!(ans > 3 || ans < 0))
        {
            return ans;
        }
        else
        {
            printf("\nPilihan tidak valid\n");
        }
    }
}

int tampilan_after_whoareu()
{
    int ans = -1;
    while (ans < 0 || ans > 3)
    {
        printf("\nWelcome to Antacha\n");
        printf("Choose one\n\n");
        printf("1. log in\n2. Create account\n3. Back\n\n=== ");
        scanf("%i", &ans);
        bersihkanBuffer();

        if (!(ans > 3 || ans < 0))
        {
            return ans;
        }
        else
        {
            printf("Pilihan tidak valid\n");
        }
    }
}

bool login_admin()
{
    char input_nama[MAX_LENGTH], input_password[MAX_LENGTH];
    printf("\n===== FORM LOG IN =====\n");

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
    printf("\n===== FORM LOG IN =====\n");

    printf("Masukkan nama: ");
    fgets(input_nama, sizeof(input_nama), stdin);
    hapusNewline(input_nama);

    printf("Masukkan password: ");
    fgets(input_password, sizeof(input_password), stdin);
    hapusNewline(input_password);

    FILE *file = fopen(FILE_AKUN, "r");
    if (!file)
        printf("\nTidak dapat membuka %s\n", FILE_AKUN);

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
    printf("\n===== FORM PENDAFTARAN =====\n");

    while (true)
    {
        printf("Masukkan nama: ");
        fgets(input_nama, sizeof(input_nama), stdin);
        hapusNewline(input_nama);

        if (user_validasi_nama_akun(input_nama))
        {
            printf("\nNama sudah terdaftar. Coba masukan nama lain.\n");
        }
        else
        {
            break;
        }
    }

    printf("Masukkan password: ");
    fgets(input_password, sizeof(input_password), stdin);
    hapusNewline(input_password);

    // daftarin akun ke data_akun.txt
    FILE *file = fopen(FILE_AKUN, "a");
    fprintf(file, "%s,%s\n", input_nama, input_password);
    fclose(file);

    printf("\nAkun berhasil didaftarkan\n");
}

void dashboard_user(int *layer, kumpulan_wahana *w_sorted, kumpulan_pesanan *L, kumpulan_pesanan *L_user, char *nama_akun_user)
{
    int ans = 0;
    do
    {
        int book_status = 0;
        int hapus = 0;
        printf("\nWelcome, user.\n");
        printf("Your upcoming explorations\n");

        user_tampilkan_pesanan(w_sorted, L_user, nama_akun_user);

        printf("\n1. See upcoming flight's\n");
        printf("2. Cancel your flight\n");
        printf("3. Simulate preparation\n");
        printf("4. Log out\n");
        printf("5. Exit\n\n=== ");
        scanf("%i", &ans);
        bersihkanBuffer();

        switch (ans)
        {
        case 1:
            sistem_tampilkan_wahana_sorted(w_sorted, 1);
            printf("\nDo you want to book ?\n[1 = yes]\n== ");
            scanf("%d", &book_status);
            bersihkanBuffer();
            user_book_flight(*w_sorted, L, L_user, book_status, nama_akun_user);
            break;
        case 2:
            printf("\nWhich flight do you want to cancel ?\n[1,2]\n== ");
            scanf("%i", &hapus);
            hapus_wahana_cancel_pesanan(2, L, L_user, w_sorted, hapus, nama_akun_user);
            break;
        case 3:
            user_simulate_preparation();
            break;
        case 4:
            sistem_sync_to_file(NULL, L, 2);
            printf("\nMenyinkronasi data...\n");
            *layer = 1;
            return;
        case 5:
            sistem_sync_to_file(NULL, L, 2);
            sistem_load_pesanan_dari_file(L);
            *layer = 4;
            return;
        default:
            printf("\ntidak ada pada pilihan\n");
            break;
        }
    } while (true);
}

void dashboard_admin(int *layer, kumpulan_wahana *L_sorted, kumpulan_pesanan *p)
{
    int ans = 0;
    do
    {
        int hapus = -1;
        printf("\nWelcome, officer.\n");
        printf("upcoming explorations\n\n");

        sistem_tampilkan_wahana_sorted(L_sorted, 2);

        printf("\n1. Add flight\n");
        printf("2. Remove flight\n");
        printf("3. Edit flight\n");
        printf("4. Execute flight\n");
        printf("5. Sync flight Data\n");
        printf("6. Log out\n");
        printf("7. Exit\n\n=== ");
        scanf("%i", &ans);
        bersihkanBuffer();

        switch (ans)
        {
        case 1:
            admin_tambah_wahana(L_sorted);
            break;
        case 2:
            printf("\nWhich flight do you want to remove ?\n==");
            scanf("%i", &hapus);
            bersihkanBuffer();
            hapus_wahana_cancel_pesanan(1, NULL, NULL, L_sorted, hapus, NULL);
            break;
        case 3:
            admin_edit_flight(L_sorted);
            break;
        case 4:
            admin_eksekusi_wahana(L_sorted, p);
            printf("\nWahana explorasi berhasil diluncurkan\n");
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
            printf("\ntidak ada pada pilihan\n");
            break;
        }
    } while (true);
}

// ## Program utama ####################################################################

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
            // tampilan buat milih as user or admin
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
            {                                        // adalah user
                int flow = tampilan_after_whoareu(); // login, create account, atau back
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
                        printf("log in tidak valid");
                    }
                    break;
                case 2:
                    user_create_account();
                    layer++;
                    break;
                case 3:
                    layer--;
                    break;
                }
            }
            else if (user_or_admin == 2)
            {
                if (login_admin())
                {
                    sistem_load_pesanan_dari_file(&list_pesanan);
                    layer++;
                }
                else
                {
                    printf("Log in tidak valid\n");
                }
            }
        }
        while (layer == 3)
        {
            int admin_answer;
            switch (user_or_admin)
            {
            case 1: // adalah user
                dashboard_user(&layer, &list_wahana_sorted, &list_pesanan, &list_pesanan_user, nama_user_login);
                break;
            case 2: // adalah admin
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
// #############################################################################################

void sistem_load_wahana_dari_file(kumpulan_wahana *L, kumpulan_wahana *L_sorted)
{
    // Initialize the linked list as empty
    L->first = NULL;

    FILE *file = fopen(FILE_WAHANA, "r");

    // Case 1: File doesn't exist
    if (!file)
    {
        printf("\nFile data_wahana.txt tidak ditemukan. Membuat file baru.\n");
        file = fopen(FILE_WAHANA, "w");
        if (file)
        {
            fclose(file);
        }
        else
        {
            printf("\nGagal membuat file data_wahana.txt\n");
        }
        return;
    }

    // Case 2: File exists but is empty
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    if (file_size == 0)
    {
        printf("\nFile data_wahana.txt kosong. Linked list dibiarkan kosong.\n");
        fclose(file);
        return;
    }
    rewind(file);

    // Case 3: File exists and has content
    char baris[500];
    int data_count = 0;

    while (fgets(baris, sizeof(baris), file))
    {
        // Skip empty lines
        if (strlen(baris) <= 1)
            continue;

        wahana *baru = (wahana *)malloc(sizeof(wahana));
        if (sscanf(baris, "%[^,],%[^,],%d,%d,%[^\n]",
                   baru->nama, baru->tanggal, &baru->tanggal_int, &baru->kursi, baru->deskripsi) == 5)
        {
            baru->next = L->first;
            L->first = baru;
            data_count++;
        }
        else
        {
            free(baru); // kalau parsing gagal, jangan ditambahkan
            printf("\nGagal memparsing baris: %s", baris);
        }
    }

    fclose(file);

    if (data_count > 0)
    {
        printf("\nBerhasil memuat %d data wahana dari file.\n", data_count);
    }
    // langsung melakukan proses sorting
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
    printf("\n===== Form Tambah Wahana =====\n\n");
    wahana *baru = malloc(sizeof(wahana));
    if (!baru)
    {
        printf("\nGagal mengalokasikan memori!\n");
        return;
    }

    // Input nama
    printf("\nMasukkan nama wahana: ");
    if (!fgets(baru->nama, sizeof(baru->nama), stdin))
    {
        printf("\nGagal membaca input nama\n");
        free(baru);
        return;
    }
    hapusNewline(baru->nama);

    // Input tanggal
    printf("\nMasukkan tanggal peluncuran (DD-MM-YY): ");
    if (!fgets(baru->tanggal, sizeof(baru->tanggal), stdin))
    {
        printf("\nGagal membaca input tanggal\n");
        free(baru);
        return;
    }
    hapusNewline(baru->tanggal);
    baru->tanggal_int = admin_konversi_tanggalInt(baru->tanggal);

    // Input kursi
    printf("Masukkan jumlah kursi: ");
    if (scanf("%d", &baru->kursi) != 1)
    {
        printf("\nInput kursi tidak valid!\n");
        bersihkanBuffer();
        free(baru);
        return;
    }
    bersihkanBuffer();

    // Input deskripsi
    printf("Masukkan deskripsi (maks 50 kata): ");
    if (!fgets(baru->deskripsi, sizeof(baru->deskripsi), stdin))
    {
        printf("\nGagal membaca input deskripsi\n");
        free(baru);
        return;
    }
    hapusNewline(baru->deskripsi);

    // Tambahkan ke linked list
    admin_transversal_add_wahana_baru(L_sorted, baru);

    // Simpan ke file
    FILE *file = fopen(FILE_WAHANA, "a");
    if (!file)
    {
        printf("\nGagal membuka file %s\n", FILE_WAHANA);
        free(baru);
        return;
    }
    fprintf(file, "%s,%s,%d,%d,%s\n",
            baru->nama, baru->tanggal,
            baru->tanggal_int, baru->kursi,
            baru->deskripsi);
    fclose(file);

    printf("\nWahana berhasil ditambahkan!\n");
}

void admin_eksekusi_wahana(kumpulan_wahana *L_sorted, kumpulan_pesanan *p)
{
    wahana *hapus = L_sorted->first;
    char target_wahana[MAX_LENGTH];
    strcpy(target_wahana, hapus->nama);

    if (L_sorted->first == NULL)
    {
        printf("\nTidak ada wahana yang tercatat\n");
        return;
    }
    else
    {
        L_sorted->first = L_sorted->first->next;
        free(hapus);
    }

    sistem_hapus_pesanan_setelah_eksekusi(p, target_wahana);

    sistem_sync_to_file(NULL, p, 2);
    sistem_sync_to_file(L_sorted, NULL, 1);
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

void admin_hapus_wahana(kumpulan_wahana *L_sorted, int hapus)
{
    wahana *prev = NULL;
    wahana *hook;

    // hapus di awal
    if (hapus == 1)
    {
        prev = L_sorted->first;
        L_sorted->first = L_sorted->first->next;
        free(prev);
        return;
    }

    // hapus di akhir
    if (hapus == L_sorted->total_node)
    {
        wahana *current = L_sorted->first;
        while (current != L_sorted->last)
        {
            prev = current;
            current = current->next;
        }
        L_sorted->last = prev;
        prev->next = NULL;
        return;
    }

    // hapus di tengah
    hook = L_sorted->first;
    int iterasi = 0;
    while (hook != NULL && !(iterasi == hapus - 1))
    {
        prev = hook;
        hook = hook->next;
        iterasi++;
    }

    if (hook != NULL)
    {
        prev->next = hook->next;
        hook->next = NULL;
        free(hook);
    }
    else
    {
        printf("\nIndex tidak ditemukan dalam list.\n");
    }
}

// selection sorting ////////////////////////////
void admin_execute_selection_sort(int total_node, kumpulan_wahana *unsorted, kumpulan_wahana *sorted)
{
    wahana *current = unsorted->first;

    sistem_empty_sorted(sorted);
    sorted->first = NULL;
    wahana *tail_sorted = NULL;

    for (int i = 0; i < total_node; i++)
    {

        current = unsorted->first;
        wahana *min = unsorted->first;
        wahana *prev_current = NULL;
        wahana *prev_min = NULL;

        // mencari node min
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

        // misahin min dari unsorted
        if (prev_min == NULL)
            unsorted->first = min->next;
        else
            prev_min->next = min->next;

        // masukin min ke sorted
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

////////////////////////////////////////////////////

void admin_transversal_add_wahana_baru(kumpulan_wahana *L_sorted, wahana *baru)
{
    if (!baru || !L_sorted)
    {
        printf("\nParameter tidak valid\n");
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

    // Cari posisi yang tepat untuk insert
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
void sistem_tampilkan_wahana_sorted(kumpulan_wahana *L_sorted, int role)
{
    if (L_sorted->first == NULL)
    {
        printf("\nNo flights found\n\n");
        return;
    }

    int count = 0;
    wahana *current = L_sorted->first;
    if (role == 2)
    {
        while (current != NULL)
        {
            printf("%d. %s\n", ++count, current->nama);
            printf("Desciption = %s\n", current->deskripsi);
            printf("Date Launch = %s\n", current->tanggal);
            printf("Seat Capasity Remain = %i\n\n", current->kursi);
            current = current->next;
        }
    }
    else if (role == 1)
    {
        while (current != NULL && current->kursi > 0)
        {
            printf("%d. %s\n", ++count, current->nama);
            printf("Desciption = %s\n", current->deskripsi);
            printf("Date Launch = %s\n", current->tanggal);
            printf("Seat Capasity Remain = %i\n\n", current->kursi);
            current = current->next;
        }
    }
}

// refresh linked list wahana sorted
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

// nulis ulang seluruh file
void sistem_sync_to_file(kumpulan_wahana *L_sorted, kumpulan_pesanan *p, int pilihan)
{
    FILE *file = NULL;

    switch (pilihan)
    {
    case 1:
        if (L_sorted == NULL || L_sorted->first == NULL)
        {
            printf("\nTidak ada data wahana untuk disinkronisasi\n");
            return;
        }

        file = fopen(FILE_WAHANA, "w");
        if (!file)
        {
            printf("\nGagal membuka file %s untuk disinkronisasi\n", FILE_WAHANA);
            return;
        }

        wahana *current = L_sorted->first;
        while (current != NULL)
        {
            fprintf(file, "%s,%s,%d,%d,%s\n", current->nama, current->tanggal, current->tanggal_int, current->kursi, current->deskripsi);
            current = current->next;
        }
        fclose(file);
        printf("\nFile has been synced");
        break;
    case 2:
        file = fopen(FILE_PESANAN, "w");
        if (!file)
        {
            printf("\nGagal membuka file %s untuk disinkronisasi\n", FILE_PESANAN);
            return;
        }

        pesanan *current_pesanan = (p ? p->first : NULL);
        while (current_pesanan)
        {
            fprintf(file, "%s,%s,%d\n", current_pesanan->nama_akun, current_pesanan->nama_wahana, current_pesanan->kursi_dipesan);
            current_pesanan = current_pesanan->next;
        }
        fclose(file);
        printf("\nData pesanan berhasil disinkronisasi\n");
        break;
    default:
        printf("\nPilihan tidak tersedia\n");
        break;
    }
}

void sistem_load_pesanan_dari_file(kumpulan_pesanan *L)
{
    // Inisialisasi ulang linked list
    free_pesanan_list(L); // Pastikan semua node lama dibebaskan
    L->first = L->last = NULL;
    L->total_pesanan = 0;

    FILE *file = fopen(FILE_PESANAN, "r");

    // kalau file tidak ada, buat baru
    if (!file)
    {
        printf("\nFile %s tidak ditemukan. Membuat file baru.\n", FILE_PESANAN);
        file = fopen(FILE_PESANAN, "w");
        if (file)
            fclose(file);
        else
            printf("\nGagal membuat file %s\n", FILE_PESANAN);
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
            printf("\nGagal mengalokasikan memori untuk node pesanan\n");
            continue;
        }

        if (sscanf(baris, "%49[^,],%49[^,],%d",
                   baru->nama_akun,
                   baru->nama_wahana,
                   &baru->kursi_dipesan) != 3)
        {
            printf("\nGagal membaca baris: %s", baris);
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

        L->total_pesanan++;
        data_count++;
    }

    fclose(file);

    printf("\nBerhasil memuat %i pesanan dari file %s\n", data_count, FILE_PESANAN);
}

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
        printf("\nUser %s belum book flight\n", nama);
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
                    printf("Desciption = %s\n", current_wahana->deskripsi);
                    printf("Date Launch = %s\n", current_wahana->tanggal);
                    printf("Seat booked = %i\n\n", current_pesanan->kursi_dipesan);
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
    char name_flight_booked[MAX_LENGTH];
    int kursi_dipesan;

    wahana *current = sorted.first;
    pesanan *baru = (pesanan *)malloc(sizeof(pesanan));
    pesanan *hook = L->first;

    printf("Masukkan nama wahana yang ingin dipesan\n== ");
    fgets(name_flight_booked, sizeof(name_flight_booked), stdin);
    hapusNewline(name_flight_booked);

    while (current != NULL)
    {
        if (strcmp(current->nama, name_flight_booked) == 0)
        {
            printf("\nMasukkan jumlah kursi yang ingin dipesan = ");
            scanf("%d", &kursi_dipesan);
            bersihkanBuffer();

            if (kursi_dipesan > current->kursi || kursi_dipesan < current->kursi)
            {
                printf("\nInput kursi tidak valid\n");
                return;
            }

            current->kursi -= kursi_dipesan;

            pesanan *p_user = (pesanan *)malloc(sizeof(pesanan));

            strcpy(p_user->nama_akun, nama);
            strcpy(p_user->nama_wahana, name_flight_booked);
            p_user->kursi_dipesan = kursi_dipesan;
            p_user->next = NULL;

            // simpan di linked list user
            if (!L_user->first)
            {
                L_user->first = p_user;
                L_user->last = p_user;
            }
            else
            {
                L_user->last = baru;
            }
            baru = NULL;

            // simpan di linked list pesanan utama
            pesanan *p_global = (pesanan *)malloc(sizeof(pesanan));
            *p_global = *p_user;
            p_global->next = NULL;

            if (L->first == NULL)
            {
                L->first = p_global;
                L->last = p_global;
                printf("\nBerhasil menambahkan pesanan di urutan 1\n");
            }
            else
            {
                L->last = p_global;
                p_global->next = NULL;
                printf("\nBerhasil menambahkan pesanan di urutan terakhir\n");
            }

            L->total_pesanan++;

            // simpan di file
            FILE *file = fopen(FILE_PESANAN, "a");
            fprintf(file, "%s,%s,%d\n", nama, current->nama, kursi_dipesan);
            fclose(file);
            printf("\nBerhasil menambahkan pesanan di file %s\n", FILE_PESANAN);
            return;
        }
        current = current->next;
    }
    printf("\nWahana tidak ditemukan\n");
}

void user_cancel_flight(kumpulan_pesanan *L, kumpulan_pesanan *L_user, char *nama_user, int hapus)
{
    pesanan *current_user = L_user->first;
    pesanan *current = L->first;
    pesanan *prev = NULL;
    int cancel_wahana;
    char target_wahana[MAX_LENGTH];

    if (cancel_wahana == 1)
    {
        strcpy(target_wahana, L_user->first->nama_wahana);
        prev = L_user->first;
        L_user->first = L_user->first->next;
        free(prev);
    }
    else if (cancel_wahana == L->total_pesanan)
    {
        while (current_user != L_user->last)
        {
            prev = current;
            current = current->next;
        }
        strcpy(target_wahana, current_user->nama_wahana);
        prev->next = NULL;
        free(current_user);
    }
    else
    {
        int iterasi = 0;
        while (current_user != NULL && !(iterasi == hapus - 1))
        {
            prev = current_user;
            current_user = current_user->next;
            iterasi++;
        }
        strcpy(target_wahana, current_user->nama_wahana);
        prev->next = NULL;
        free(current);
    }
    L->total_pesanan -= 1;
    L_user->total_pesanan -= 1;

    // hapus di linked list bukan user
    current = L->first;
    prev = NULL;
    while (current != NULL)
    {
        if (strcmp(current->nama_wahana, target_wahana) == 0 && strcmp(current->nama_akun, nama_user) == 0)
        {
            if (prev == NULL)
            {
                L->first = L->first->next;
                prev = NULL;
                free(prev);
            }
            else
            {
                if (current == L->last)
                {
                    L->last = prev;
                    L->last->next = NULL;
                    free(current);
                }
                else
                {
                    prev->next = current->next;
                    free(current);
                }
            }
        }
        prev = current;
        current = current->next;
    }
}

void hapus_wahana_cancel_pesanan(int pilihan, kumpulan_pesanan *L, kumpulan_pesanan *L_user, kumpulan_wahana *W_sorted, int hapus, char *nama)
{
    if (pilihan == 1)
    {
        admin_hapus_wahana(W_sorted, hapus);
    }
    else if (pilihan == 2)
    {
        user_cancel_flight(L, L_user, nama, hapus);
    }
    else
    {
        printf("\nTidak ada di pilihan\n");
    }
    return;
}

bool admin_cek_wahana_dipesan(char *nama_wahana)
{
    FILE *file = fopen(FILE_WAHANA, "r");
    if (!file)
        return false;

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
    printf("Masukkan nama wahana yang ingin diedit: ");
    fgets(target_wahana, sizeof(target_wahana), stdin);
    hapusNewline(target_wahana);

    if (admin_cek_wahana_dipesan(target_wahana))
    {
        printf("\nWahana ini sudah dipesan, regulasi membatasi wewenang admin untuk melakukan edit.\n");
        return;
    }

    wahana *current = L_sorted->first;
    while (current != NULL)
    {
        if (strcmp(current->nama, target_wahana) == 0)
        {
            printf("Edit data untuk wahana %s\n", current->nama);

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
            return;
        }
        current = current->next;
    }
    printf("\nWahana tidak ditemukan\n");
}

void free_wahana_list(kumpulan_wahana *L)
{
    wahana *current = L->first, *tmp;
    while (current)
    {
        tmp = current->next;
        free(current);
        current = tmp;
    }
    L->first = L->last = NULL;
}

void free_pesanan_list(kumpulan_pesanan *L)
{
    pesanan *current = L->first, *tmp;
    while (current)
    {
        tmp = current->next;
        free(current);
        current = tmp;
    }
    L->first = L->last = NULL;
}

void user_simulate_preparation()
{
    int choice;
    printf("\n===== MODUL SIMULASI LATIHAN ASTRONOT =====\n");
    printf("\nSelamat datang, calon astronot! Hari ini Anda akan mengikuti dua latihan penting\n");
    printf("untuk mengasah kemampuan navigasi dan pengoptimalan sistem dalam misi luar angkasa.\n");
    printf("Pilih tantangan yang ingin Anda kerjakan:\n\n");

    printf("1. Optimasi sistem kapal [algoritma Kruskal]\n");
    printf("2. Mencari jalur tercepat [algoritma Dijkstra]\n");
    printf("3. Kembali ke dashboard\n");
    printf("=== ");
    scanf("%d", &choice);
    bersihkanBuffer();

    switch (choice)
    {
    case 1:
    {
        printf("\n--- Tantangan 1: Optimasi sistem kapal luar angkasa ---\n");
        printf("Detail misi: Kapal Anda memiliki beberapa modul yang saling terhubung.\n");
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
        printf("\nJawaban Anda: ");
        int cek = scanf("%d", &jawaban);
        bersihkanBuffer();
        if (cek != 1)
        {
            printf("\nInput tidak valid. Masukkan angka bulat.\n");
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
        printf("\n--- Tantangan 2: Menentukan jalur tercepat ke modul tujuan ---\n");
        printf("Detail misi: Kendali misi ingin mengetahui jalur tercepat bagi shuttle suplai\n");
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
        printf("\nJawaban Anda (dalam menit): ");
        int cek = scanf("%d", &jawaban);
        bersihkanBuffer();
        if (cek != 1)
        {
            printf("\nInput tidak valid. Masukkan angka bulat.\n");
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
        printf("\nPilihan tidak valid. Silakan pilih 1, 2, atau 3.\n");
        break;
    }
}
