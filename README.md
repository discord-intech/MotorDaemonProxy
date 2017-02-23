# MotorDaemonProxy
Proxy for MotorDaemon in case INTechOS is located in a NAT-restricted environment

MotorDaemonConsole/Controller simply has to specify MotorDaemonProxy's IP as it was the actual MotorDaemon.

                             +
+---------------------+      |               +------------------------+                     +-------------------------+
|                     |      |               |                        |                     |                         |
|                     +---------------------->                        +--------------------->    MotorDaemonConsole   |
|  MotorDaemon -p IPb |      |    Port 56990 |    MotorDaemonProxy    | Port 56987          |           or            |
|                     |      |               |                        |                     |   MotorDaemonController |
|                     <----------------------+                        <---------------------+                         |
|         IPa         |      |               |           IPb          |                     |           IPc           |
+---------------------+      |               +------------------------+                     +-------------------------+
                             |
                             |
                             |Annoying NAT
                             +

