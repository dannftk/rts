# Configuration

1. Change the active directory to the root of the **RTS** project where the main _CMakeLists.txt_ is located
2. Create a directory where you're going to build the project, for example:

        bash> mkdir build
3. Change the active directory to the new created _build_ and run:

        bash> cmake ../

    This will configure the project by means of cmake

4. Start building the project by running the command:

        bash> cmake --build .

    This will build the project including all the subprojects like _mclient_, _vclient_, _gateway_, _deststation_

    If there are issues while building contact dannftk@yandex.ru

5. As just you have built the project, you will have 4 applications:

    * gateway (it is resposible for main and concurent operations-calculations)
    * deststation (do not confuse with destination, it is another word :) Actually, I personally used to confuse them :)
    this applications is requesting values of the ending vector from gateway
    and getting results from it on finishing)
    * mclient (provides values of a matrix)
    * vlcient (provides values of a vector)

6. Copy the binaries built (gateway, deststation, mclient, vclient) to any same directory, then run the applications as background processes (you could leave the last one as a foreground, as you want).
    Gateway process is supposed to be run the first:

        bash> ./gateway &
        bash> ./destation &
        bash> ./mclient ../files/matrix_A.txt 0 mtrx_A &
        bash> ./mclient ../files/matrix_C.txt 1 mtrx_C &
        bash> ./vclient ../files/vector_b.txt 2 vec_b &
        bash> ./vclient ../files_vector_d.txt 3 vec_d &

Do not confuse the parameters and their order!
It was a fast-hand applications, sorry, I couldn't have been on time to make it perfect

Logging is on and stowed in the same directory where the applications are launched, logs are produced by each application
Actually there is one log more from 'dispatcher' of the gateway being written to the same directory where binaries are located

The applications comunicate by means UNIX socket technology

Read logs and enjoy. Actually the gist was to demonstrate that this system meets the requrements to be called 'real time'
But, you know, it is a sophistication and simulation
But, actually, it resembles RTS and shows how gateway is trying to be in time!

If there are any issues contact dannftk@yandex.ru
