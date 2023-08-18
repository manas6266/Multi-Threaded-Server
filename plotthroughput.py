import subprocess
import matplotlib.pyplot as plt
fig,axis=plt.subplots(2)
users=100
itr=15
throughput=list()
avg_RTT=[]
no_of_user=list()
for i in range(itr):
    test = subprocess.Popen(["./load_gen","localhost","8087",str(users),"0.1","60"], stdout=subprocess.PIPE)
    output = test.communicate()[0]
    print(output)
    ab=output.decode("ascii")
    print(ab)
    
    no_of_user.append(users)
    throughput.append(float(ab.split("\n")[5].split(":")[1].strip()))
    avg_RTT.append(float(ab.split("\n")[6].split(":")[1].strip()))
    print(throughput)
    axis[0].plot(no_of_user,throughput)
    axis[0].set_title("Throughputhput")
    axis[0].set_xlabel("no of users")
    axis[0].set_ylabel("throughput")
    axis[1].plot(no_of_user,avg_RTT)
    axis[1].set_title("Avg RTT")
    axis[1].set_xlabel("no of users")
    axis[1].set_ylabel("avg response")
    
    plt.tight_layout()

    plt.savefig("throughput_graph.png")

    users+=500