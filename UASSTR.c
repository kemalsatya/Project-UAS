#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LENGTH 50
#define MAX_CEK_AKUN 50
#define MAX_EXPLORASI 10

#define FILE_AKUN "data_akun.txt"
#define FILE_WAHANA "data_wahana.txt"

// Log in untuk admin ///////////////////////////////////////////////////
char nama_admin[] = "admin";
char pass_admin[] = "admin1234";

// Beberapa fungsi kecil yang bisa dipakai ///////////////////////////////////
void hapusNewline(char *str) {
    str[strcspn(str, "\n")] = 0;
}

void bersihkanBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

bool validasi_nama_akun(char *username){
    FILE *file = fopen(FILE_AKUN, "r");
    if (file == NULL) {
        return false;
    }

    char line[MAX_LENGTH * 2]; //menampung akun + password

    while (fgets(line, sizeof(line), file) ){
        hapusNewline(line);
        char username_terdaftar[MAX_CEK_AKUN];
        sscanf(line, "%[^,]", username_terdaftar);

        if (strcmp(username_terdaftar, username) == 0){
            fclose(file); //
            return true;
        }
    }
    fclose(file);
    return false;
}

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

// struct penting /////////////////////////////////////////////////////////////
typedef struct wahana {
    char nama[MAX_LENGTH];
    char tanggal[MAX_LENGTH]; //format DD-MM-YY untuk ditampilin
    int tanggal_int; // untuk sorting
    int kursi;
    char deskripsi[300]; // cukup 50 kata
    struct wahana *next;
} wahana;

typedef struct kumpulan_wahana {
    wahana *first;
    wahana *last;
    int total_node;
} kumpulan_wahana;

void load_sort_wahana_dari_file(kumpulan_wahana *L, kumpulan_wahana *L_sorted);

void init_wahana(kumpulan_wahana *L){
    L->first = NULL;
    L->last = NULL;
    L->total_node = 0;
}

int konversi_tanggalInt(const char *tanggal); //oleh admin untuk tambah wahana

void empty_sorted(kumpulan_wahana *L_sorted);
void tambah_wahana(kumpulan_wahana *L_sorted);
void hapus_wahana(kumpulan_wahana *L_sorted, int hapus);
void eksekusi_wahana(kumpulan_wahana *L_sorted);
void sync_to_file(kumpulan_wahana *L_sorted);
void ask_for_sync(kumpulan_wahana *L_sorted){
    int ans;
    printf("\nDo you want to sync file before leave ?\n[1 = yes]\n== ");
    scanf("%i", &ans);
    if (ans == 1){
        sync_to_file(L_sorted);
    }
}

// sorting  //////////
void execute_selection_sort(int total_node,kumpulan_wahana *unsorted, kumpulan_wahana *sorted);
void transversal_add_wahana_baru(kumpulan_wahana *L_sorted, wahana *baru);
void tampilkan_wahana_sorted(kumpulan_wahana *L_sorted);
////////////////////////////////

// searching ///////

////////////////////


int tampilan_whoareu(){
    int ans = -1;
    while(ans > 3 || ans < 0){
        printf("Welcome to Antacha\n");
        printf("Who are you ?\n");
        printf("1. User\n2. Admin\n3. Exit program\n[1/2/3]\n");
        printf("\n== ");
        scanf("%i", &ans);
        bersihkanBuffer();

        if(!(ans >3 || ans < 0)){
            return ans;
        } else {
            printf("Pilihan tidak valid\n");
        }
    }
}

int tampilan_after_whoareu(){
    int ans = -1;
    while(ans < 0 || ans > 3){
        printf("Welcome to Antacha\n");
        printf("Choose one\n\n");
        printf("1. log in\n2. Create account\n3. Back\n\n=== ");
        scanf("%i", &ans);
        bersihkanBuffer();

        if(!(ans >3 || ans < 0)){
                return ans;
        } else {
                printf("Pilihan tidak valid\n");
        }
    }
}

bool login_admin(){
    char input_nama[MAX_LENGTH], input_password[MAX_LENGTH];
    printf("\n===== FORM LOG IN =====\n");

    printf("Masukkan nama: ");
    fgets(input_nama, sizeof(input_nama), stdin); hapusNewline(input_nama);

    printf("Masukkan password: ");
    fgets(input_password, sizeof(input_password), stdin); hapusNewline(input_password);

    if (strcmp(nama_admin, input_nama) == 0 && strcmp(pass_admin, input_password) == 0){
        return true;
    } else {
        return false;
    }
}

bool login_user(){
    char input_nama[MAX_LENGTH], input_password[MAX_LENGTH];
    printf("\n===== FORM LOG IN =====\n");

    printf("Masukkan nama: ");
    fgets(input_nama, sizeof(input_nama), stdin); hapusNewline(input_nama);

    printf("Masukkan password: ");
    fgets(input_password, sizeof(input_password), stdin); hapusNewline(input_password);

    FILE *file = fopen(FILE_AKUN, "r");
    if (!file) return false;

    char line[MAX_CEK_AKUN * 2];
    while(fgets(line, sizeof(line), file)){
        char iterasi_nama_file[MAX_LENGTH], iterasi_password_file[MAX_LENGTH];
        sscanf(line, "%[^,],%s", iterasi_nama_file, iterasi_password_file);
        if (strcmp(iterasi_nama_file, input_nama) == 0 && 
            strcmp(iterasi_password_file, input_password) == 0){
            fclose(file);
            return true;
        }
    }
    fclose(file);
    return false;
}

void create_account_user(){
    char input_nama[MAX_LENGTH], input_password[MAX_LENGTH];
    printf("\n===== FORM PENDAFTARAN =====\n");

    while(true){
        printf("Masukkan nama: ");
        fgets(input_nama, sizeof(input_nama), stdin);
        hapusNewline(input_nama);

        if(validasi_nama_akun(input_nama)){
            printf("Nama sudah terdaftar. Coba masukan nama lain.\n");
        } else {
            break;
        }
    }

    printf("Masukkan password: ");
    fgets(input_password, sizeof(input_password), stdin);
    hapusNewline(input_password);

    //daftarin akun ke data_akun.txt
    FILE *file = fopen(FILE_AKUN, "a");
    fprintf(file, "%s,%s\n", input_nama, input_password);
    fclose(file);

    printf("\nAkun berhasil didaftarkan\n");
}

void dashboard_user(int *layer){
    int ans = 0;
    do
    {
        printf("\nWelcome, user.\n");
        printf("Your upcoming explorations\n");

        printf("... [Belum dibuat wahananya]\n");

        printf("1. See upcoming flight's\n");
        printf("2. Cancel your flight\n");
        printf("3. Simulate preparation\n");
        printf("4. Log out\n");
        printf("5. Exit\n\n=== ");
        scanf("%i", &ans);
        bersihkanBuffer();

        switch(ans){
            case 1: printf("Anda memilih 1"); break;
            case 2: printf("Anda memilih 2"); break;
            case 3: printf("Anda memilih 3"); break;
            case 4: *layer = 1; break;
            case 5: *layer = 4; break;
            default : printf("tidak ada pada pilihan\b"); break;
        }
    } while (ans < 1 || ans > 5);
}

void dashboard_admin(int *layer, kumpulan_wahana *L, kumpulan_wahana *L_sorted){
    int ans = 0;
    do
    {
        int hapus = -1;
        printf("\nWelcome, officer.\n");
        printf("upcoming explorations\n\n");

        tampilkan_wahana_sorted(L_sorted);

        printf("1. Add flight\n");
        printf("2. Remove flight\n");
        printf("3. Execute flight\n");
        printf("4. Sync flight Data\n");
        printf("5. Log out\n");
        printf("6. Exit\n\n=== ");
        scanf("%i", &ans);
        bersihkanBuffer();

        switch(ans){
            case 1: tambah_wahana(L_sorted); break;
            case 2: 
                printf("\nWhich flight do you want to remove ?\n=="); 
                scanf("%i", &hapus); bersihkanBuffer();
                hapus_wahana(L_sorted, hapus); 
                break;
            case 3: 
                eksekusi_wahana(L_sorted);
                printf("\nWahana explorasi berhasil diluncurkan\n");
                break;
            case 4: sync_to_file(L_sorted); break;
            case 5: ask_for_sync(L_sorted); *layer = 1; break;
            case 6: ask_for_sync(L_sorted); *layer = 4; break;
            default : printf("tidak ada pada pilihan\n"); break;
        }
    } while (ans < 1 || ans > 6);
}

//## Program utama ####################################################################

int main(){
    int system = 1;
    int layer = 1;
    kumpulan_wahana list_wahana;
    kumpulan_wahana list_wahana_sorted;

    init_wahana(&list_wahana);
    init_wahana(&list_wahana_sorted);
    load_sort_wahana_dari_file(&list_wahana, &list_wahana_sorted);

while(system){
    int user_or_admin = -1;
    // 1 = user, 2 = admin
    while(layer == 1){
        //tampilan buat milih as user or admin
        user_or_admin = tampilan_whoareu();
        switch(user_or_admin){
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
    while(layer == 2){
        if(user_or_admin == 1){ // adalah user
            int flow = tampilan_after_whoareu(); // login, create account, atau back
            switch(flow){
                case 1:
                    if(login_user()){
                        layer++;
                    } else {
                        printf("log in tidak valid");
                    }
                    break;
                case 2: create_account_user(); layer++; break;
                case 3: layer--; break;
            }
        }
        else if (user_or_admin == 2){
            if(login_admin()){
                layer++;
            } else {
                printf("Log in tidak valid\n");
            }
        }
    }
    while(layer == 3){
        int admin_answer;
        switch(user_or_admin){
            case 1 : //adalah user
            dashboard_user(&layer); break;
            case 2 : //adalah admin
            dashboard_admin(&layer, &list_wahana, &list_wahana_sorted); break;
        }
    }
    while(layer == 4){
        return 0;
    }
}
}
//#############################################################################################


void load_sort_wahana_dari_file(kumpulan_wahana *L, kumpulan_wahana *L_sorted) {
    // Initialize the linked list as empty
    L->first = NULL;
    
    FILE *file = fopen(FILE_WAHANA, "r");
    
    // Case 1: File doesn't exist
    if (!file) {
        printf("⚠️  File data_wahana.txt tidak ditemukan. Membuat file baru.\n");
        file = fopen(FILE_WAHANA, "w");
        if (file) {
            fclose(file);
        } else {
            printf("⚠️  Gagal membuat file data_wahana.txt\n");
        }
        return;
    }
    
    // Case 2: File exists but is empty
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    if (file_size == 0) {
        printf("ℹ️  File data_wahana.txt kosong. Linked list dibiarkan kosong.\n");
        fclose(file);
        return;
    }
    rewind(file);
    
    // Case 3: File exists and has content
    char baris[500];
    int data_count = 0;
    
    while (fgets(baris, sizeof(baris), file)) {
        // Skip empty lines
        if (strlen(baris) <= 1) continue;
        
        wahana *baru = (wahana *)malloc(sizeof(wahana));
        if (sscanf(baris, "%[^,],%[^,],%d,%d,%[^\n]",
                   baru->nama, baru->tanggal, &baru->tanggal_int, &baru->kursi, baru->deskripsi) == 5) {
            baru->next = L->first;
            L->first = baru;
            data_count++;
        } else {
            free(baru); // kalau parsing gagal, jangan ditambahkan
            printf("⚠️  Gagal memparsing baris: %s", baris);
        }
    }
    
    fclose(file);
    
    if (data_count > 0) {
        printf("✅  Berhasil memuat %d data wahana dari file.\n", data_count);
    }
    // langsung melakukan proses sorting
    execute_selection_sort(data_count, L, L_sorted);

}

int konversi_tanggalInt(const char *tanggal) {
    int dd, mm, yy;
    sscanf(tanggal, "%d-%d-%d", &dd, &mm, &yy);
    yy += (yy < 50) ? 2000 : 1900;
    return yy * 10000 + mm * 100 + dd;
}

void tambah_wahana(kumpulan_wahana *L_sorted){
    printf("\n===== Form Tambah Wahana =====\n\n");
    wahana *baru = (wahana *)malloc(sizeof(wahana));

    printf("Masukkan nama wahana: ");
    fgets(baru->nama, sizeof(baru->nama), stdin);
    hapusNewline(baru->nama);

    printf("Masukkan tanggal peluncuran (DD-MM-YY): ");
    fgets(baru->tanggal, sizeof(baru->tanggal), stdin);
    hapusNewline(baru->tanggal);

    baru->tanggal_int = konversi_tanggalInt(baru->tanggal);

    printf("Masukkan jumlah kursi: ");
    scanf("%d", &baru->kursi);
    bersihkanBuffer();

    printf("Masukkan deskripsi (maks 50 kata): ");
    fgets(baru->deskripsi, sizeof(baru->deskripsi), stdin);
    hapusNewline(baru->deskripsi);

    transversal_add_wahana_baru(L_sorted, baru);

    FILE *file = fopen(FILE_WAHANA, "a");
    if(file){
        fprintf(file, "%s,%s,%d,%d,%s\n", baru->nama, baru->tanggal, baru->tanggal_int, baru->kursi, baru->deskripsi);
        fclose(file);
        printf("Flight has been added\n");
    } else {
        printf("Cannot add nes flight");
    }

}

void eksekusi_wahana(kumpulan_wahana *L_sorted){
    wahana *prev = NULL;
    if(L_sorted->first == NULL){
        printf("\nTidak ada wahana yang tercatat\n");
        return;
    } else {
        prev = L_sorted->first;
        L_sorted->first = L_sorted->first->next;
        free(prev);
    }
}

void hapus_wahana(kumpulan_wahana *L_sorted, int hapus){
    wahana *prev = NULL;
    wahana *hook;
    
    //hapus di awal
    if(hapus == 1){
        prev = L_sorted->first;
        L_sorted->first = L_sorted->first->next;
        free(prev);
        return;
    }

    //hapus di akhir
    if(hapus == L_sorted->total_node){
        wahana *current = L_sorted->first;
        while(current != L_sorted->last){
            prev = current;
            current = current->next;
        }
        L_sorted->last = prev;
        prev->next = NULL;
        return;
    }

    //hapus di tengah
    hook = L_sorted->first;
    int iterasi = 0;
    while(hook != NULL && !(iterasi == hapus-1)){
        prev = hook;
        hook = hook->next;
        iterasi++;
    }

    if (hook != NULL) {
        prev->next = hook->next;
        hook->next = NULL;
        free(hook);
    } else {
        printf("⚠️ Index tidak ditemukan dalam list.\n");
    }

}

// selection sorting ////////////////////////////

void execute_selection_sort(int total_node, kumpulan_wahana *unsorted, kumpulan_wahana *sorted){
    wahana *current = unsorted->first;
  
    empty_sorted(sorted);
    sorted->first = NULL;
    wahana *tail_sorted = NULL;

    for (int i = 0; i < total_node; i++){

        current = unsorted->first;
        wahana *min = unsorted->first;
        wahana *prev_current = NULL;
        wahana *prev_min = NULL;

        //mencari node min
        while (current != NULL) 
        {
            if (current->tanggal_int < min->tanggal_int){
                min = current;
                prev_min = prev_current;
            }

            prev_current = current;
            current = current->next;
        }

        //misahin min dari unsorted
        if(prev_min == NULL) unsorted->first = min->next;
        else prev_min->next = min->next;
        
        //masukin min ke sorted
        if (sorted->first == NULL){
            sorted->first = min;
            tail_sorted = min;
        } else {
            tail_sorted->next = min;
            tail_sorted = min;
        }
        min->next = NULL;
    }
}

void transversal_add_wahana_baru(kumpulan_wahana *L_sorted, wahana *baru){
    wahana *current = L_sorted->first;
    wahana *prev = NULL;

    // kalau kosong
    if(L_sorted->first == NULL){
        baru->next = NULL;
        L_sorted->first = baru;
        L_sorted->last = baru;
        return;
    }

    // kalau di akhir
    if(baru->tanggal_int >= L_sorted->last->tanggal_int){
        L_sorted->last->next = baru;
        baru->next = NULL;
        L_sorted->last = baru;
        return;
    }

    while(current != NULL){
        if(baru->tanggal_int < current->tanggal_int){
            if(prev == NULL){
                // ternyata di posisi pertama
                baru->next = L_sorted->first;
                L_sorted->first = baru;
                return;
            }
            // posisi di tengah
            prev->next = baru;
            baru->next = current;

            //update jumlah node
            L_sorted->total_node++;
            printf("Node berhasil dimasukkan ke dalam list_wahana_sorted\n");
            return;
        }
        prev = current;
        current = current->next;
    }
}

//L_sorted hanya ditampilkan di dashboard admin, dan saat user book flight
void tampilkan_wahana_sorted(kumpulan_wahana *L_sorted){ 
    if(L_sorted->first == NULL){
        printf("\nNo flights found\n\n");
        return;
    }

    int count = 0;
    wahana *current = L_sorted->first;
    while (current != NULL)
    {
        printf("%d. %s\n", ++count, current->nama);
        printf("Desciption = %s\n", current->deskripsi);
        printf("Date Launch = %s\n", current->tanggal);
        printf("Seat Capasity = %i\n\n", current->kursi);
        current = current->next;
    }   
}

void empty_sorted(kumpulan_wahana *L_sorted) {
    wahana *current = L_sorted->first;
    while(current != L_sorted->last){
        wahana *temp = current;
        current = current->next;
        free(temp);
    }
    L_sorted->first = NULL;
    L_sorted->last = NULL;
    L_sorted->total_node = 0;
}

// nulis ulang seluruh file data_wahana.txt
void sync_to_file(kumpulan_wahana *L_sorted){
    wahana *current = L_sorted->first;
    FILE *file = fopen(FILE_WAHANA, "w");
    for(int i = 0; i < L_sorted->total_node; i++){
        fprintf(file, "%s,%s,%d,%d,%s\n", current->nama, current->tanggal, current->tanggal_int, current->kursi, current->deskripsi);
    }
    fclose(file);
    printf("\nFile has been synced");
}
