import pandas as pd
import matplotlib.pyplot as plt

plt.xlabel('Tempo (segundos)')
plt.ylabel('Aumento de Temperatura (°C)')
plt.title('Aumento da temperatura dos nós')

for i in range(6):
    df = pd.read_csv('temperature_stats_node_'+str(i)+'.txt', sep="\t", header=None)
    df[0] = df[0].apply(lambda x: int(x))
    plt.plot(df[0], df[1], label='Nó '+str(i))
    plt.legend(loc='center left', bbox_to_anchor=(1, 0.5))
    #plt.show()
    plt.savefig('temperatura_temporal.png', dpi=500, bbox_inches='tight', pad_inches=1)
