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
    3. Data wahana -> wahana.txt

    isi file : 
    1. list_akun.txt -> Nama : [nama_user] | Password : [password_user]
    2. pesanan.txt -> Nama: Rani | Wahana: Casio moon rocket
    3. wahana.txt -> judul, deskripsi, tanggal keberangkatan, kursi.

    isi file wahana.txt sama pesanan.txt bakal diambil buat ditaroh di linked list, biar bisa ditampilin
    wahana.txt => ditambah, dihapus, dicari data kursi yang kosong
    pesanan.txt => ditampilin pesanan kursi user, hapus pesanan user
## 
*/

// struct penting /////////////////////////////////////////////////////////////
typedef struct wahana {
    char nama[MAX_LENGTH];
    char tanggal[MAX_LENGTH]; //format DD-MM-YY
    int kursi;
    char deskripsi[300]; // cukup 50 kata
    struct wahana *next;
} wahana;

typedef struct kumpulan_wahana {
    wahana *first;
} kumpulan_wahana;

void load_wahana(kumpulan_wahana *head);

void init_wahana(){
    kumpulan_wahana list_wahana;
    list_wahana.first = NULL;

    FILE *file = fopen("wahana.txt", "a");
    fclose(file);

    load_wahana(&list_wahana);  
}


// Fungsi-fungsi fitur program /////////////////////////////////////////////////
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

    bool validasi_nama = strcmp(nama_admin, input_nama);
    bool validasi_password = strcmp(pass_admin, input_password);
    if (validasi_nama == 0 && validasi_password == 0){
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
        if (strcmp(iterasi_nama_file, input_nama) == 0){
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

void dashboard_admin(int *layer){
    int ans = 0;
    do
    {
        printf("\nWelcome, officer.\n");
        printf("upcoming explorations\n");

        printf("... [Belum dibuat wahananya]\n");
        
        printf("1. Add flight\n");
        printf("2. Remove flight\n");
        printf("3. Execute flight\n");
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
    } while (ans < 1 || ans > 6);
    
}

// Program utama /////////////////////////////////////////////////////////////////////////

int main(){
    int system = 1;
    int layer = 1;

    init_wahana();

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
                layer++; //3
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
            dashboard_admin(&layer); break;
        }
    }
    while(layer == 4){
        return 0;
    }
}
}
//////////////////////////////////////////////////////////////////////////////////////////////


void load_wahana(kumpulan_wahana *L){
    FILE *file = fopen("wahana.txt", "r");
    if (!file) {
        printf("⚠️  Gagal membuka file wahana.txt\n");
        return;
    }

    char baris[500];
    while (fgets(baris, sizeof(baris), file)) {
        wahana *baru = (wahana *)malloc(sizeof(wahana));
        if (sscanf(baris, "%[^,],%[^,],%d,%[^\n]", 
                   baru->nama, baru->tanggal, &baru->kursi, baru->deskripsi) == 4) {
            baru->next = L->first;
            L->first = baru;
        } else {
            free(baru); // kalau parsing gagal, jangan ditambahkan
        }
    }

    fclose(file);
}