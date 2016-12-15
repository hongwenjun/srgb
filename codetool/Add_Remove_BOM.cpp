// 列表文件增加 BOM
for (auto it = lst.begin(); it != lst.end(); it++)
{
    FILE* f = fopen(it->wholePath.c_str(), "rb");
    if (!f) {
        cerr << "Error: Cannot open" << it->wholePath << endl;
        continue;
    }
    unsigned char buf[10];
    fread(buf, 1, 3, f);

    // Get file size:
    fseek(f, 0, SEEK_END);
    const long N = ftell(f);
    fclose(f);

    if (buf[0] == 0xEF && buf[1] == 0xBB && buf[2] == 0xBF) {
        cout  << "Skipping (already has BOM): " << it->wholePath << endl;
        continue;
    }

    // Add BOM: =======================================
    // Read the whole file:
    unsigned char* fil_buf = new unsigned char[N + 3];
    fil_buf[0] = 0xEF;
    fil_buf[1] = 0xBB;
    fil_buf[2] = 0xBF;

    f = fopen(it->wholePath.c_str(), "rb");
    fread(fil_buf + 3, 1, N, f);
    fclose(f);

    // Write new version:
    f = fopen(it->wholePath.c_str(), "wb");
    fwrite(fil_buf, 1, N + 3, f);
    fclose(f);

    delete[] fil_buf;

    cout << "Added BOM: " << it->wholePath << endl;
}


// 列表文件移除 BOM
for (auto it it = lst.begin(); it != lst.end(); it++)
{
    FILE* f = fopen(it->wholePath.c_str(), "rb");
    if (!f) {
        cerr << "Error: Cannot open" << it->wholePath << endl;
        continue;
    }
    unsigned char buf[10];
    fread(buf, 1, 3, f);

    // Get file size:
    fseek(f, 0, SEEK_END);
    const long N = ftell(f);
    fclose(f);

    if (!(buf[0] == 0xEF && buf[1] == 0xBB && buf[2] == 0xBF)) {
        cout  << "Skipping (has no BOM): " << it->wholePath << endl;
        continue;
    }

    // Remove BOM: =======================================
    // Read the whole file:
    unsigned char* fil_buf = new unsigned char[N];

    f = fopen(it->wholePath.c_str(), "rb");
    fread(fil_buf, 1, N, f);
    fclose(f);

    // Write new version:
    f = fopen(it->wholePath.c_str(), "wb");
    fwrite(fil_buf + 3, 1, N - 3, f);
    fclose(f);

    delete[] fil_buf;

    cout << "Removed BOM: " << it->wholePath << endl;
}


















