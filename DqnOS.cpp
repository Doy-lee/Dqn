struct DqnProcess
{
};

void DqnOS_Test()
{
    // pid_t pid = vfork();
    // if (pid == 0)
    // {
    //     char const *argv[] = {"jim", "jams", NULL};
    //     char const *envp[] = {"some", "environment", NULL};
    //     chdir("/home/usr/loki/");
    //     execve(cmd, argv, envp);
    //     perror("Could not execute");
    // }
#if defined(DQN__IS_WIN32)
    // CreateProcessW();
    defer(printf("hello world\n"));

    WSAData wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0)
    {
        fprintf(stderr, "WSAStartup failed: %d\n", result);
        return;
    }

    addrinfo hints    = {};
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    addrinfo *foundAddr = nullptr;
    result = getaddrinfo("127.0.0.1", "38151", &hints, &foundAddr);
    if (result != 0)
    {
        fprintf(stderr, "getaddrinfo failed: %d\n", result);
        WSACleanup();
        return;
    }

    SOCKET connectSocket = socket(foundAddr->ai_family, foundAddr->ai_socktype, foundAddr->ai_protocol);
    if (connectSocket == INVALID_SOCKET)
    {
        fprintf(stderr, "socket failed: %d\n", WSAGetLastError());
        freeaddrinfo(foundAddr);
        WSACleanup();
        return;
    }
#endif
}
