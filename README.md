# Global-State_Aware_Automatic_NUMA_Balancing
Non-uniform memory access (NUMA) has become a standard architecture for modern servers. However, NUMA effect (i.e., local memory access typically takes shorter time than remote memory accesses) is unavoidable. To address this issue, Automatic NUMA Balancing(Auto-NUMA) was proposed. Nevertheless, Auto-NUMA can improve or hurt performance of an application, depending on its characteristics which is difficult for end users to know.

To tackle this problem, we propose Global-State Aware Automatic NUMA Balancing (GSA-Auto-NUMA). It innovates two techniques. First, GSA-Auto-NUMA identifies a set of key metrics to accurately assess the current state of a NUMA system. Second, GSA-Auto-NUMA utilizes these metrics to make real-time decisions on whether to enable Auto-NUMA through five steps of evaluation.

Please cite the following paper if you use Global-State Aware Automatic NUMA Balancing:

Jianguo Liu and Zhibin Yu. 2024. Global-State Aware Automatic NUMA Balancing. In Proceedings of the 15th Asia-Pacific Symposium on Internetware (Internetware '24). Association for Computing Machinery, New York, NY, USA, 317–326. https://doi.org/10.1145/3671016.3671380

## How to Use
