## nxp-servo
### nxp-servo Introduction 

**nxp-servo** is a CiA402 (also referred to as DS402) profile framework based on Igh CoE interface( An EtherCAT Master stack, see _EtherCAT_ section for details ), and it abstracts the CiA 402 profile and provides a easy-used API  for App developer. 

the `nxp-servo` project consists of a basic library _libnservo_  and several  auxiliary tools.

The application developed with _libnservo_ is flexible enough to adapt to  the changing of CoE network by modifying the *xml* config file  which is loaded when the application starts. The _xml_ config file describes the necessary information, including EtherCAT network topology,  slaves configurations, masters configurations and all axles definitions.

### CoE Network

A typical CoE network is shown as figure below:

![1576567023117](_images\1576567023117.png)

There are three CoE servo on this network and we name them slave _**x**_ as the position they are.  Each CoE servo could have more then one axle.  The libnservo will initiate the CoE network and encapsulate the detail of  network topology into axle nodes.  So the developer could focus on the each axle operation without taking care of the network topology.

### libnservo Architecture



![1576479531931](_images\1576479531931.png)

*nxp-servo* is running on top of `Igh` EtherCAT stack. And the `Igh` stack provides  CoE communication mechanisms - Mailbox and Process Data.  Using these mechanisms, *nxp-servo* could access the CiA Object Dictionary located on CoE servo.

Control task initiates the master, all slaves on the CoE network and registers all PDOs to Igh stack, then constructs a data structure to describe each axle. Finally , the control task creates a task to run the user task periodically.


### XML Config

This section focuses on how the xml config file describes a CoE network.

the skeleton of XML config is shown as in figure below:

```
<?xml version="1.0" encoding="utf-8"?>
<Config  Version="1.2">
	<PeriodTime>#10000000</PeriodTime>
	<MaxSafeStack>#8192</MaxSafeStack>
	<master_status_update_freq>#1</master_status_update_freq>
	<slave_status_update_freq>#1</slave_status_update_freq>
	<axle_status_update_freq>#1</axle_status_update_freq>
	<sync_ref_update_freq>#2</sync_ref_update_freq>
	<is_xenomai>#1</is_xenomai>
	<sched_priority>#82</sched_priority>
	<Masters>
		<Master>
			...
		<\Master>
		<Master>
			...
		<\Master>
    <\Master>
	
	<Axles>
    	<Axle>
    		...
    	<\Axle>
    	<Axle>
    		...
    	<\Axle>
    <\Axles>	
</Config>
```

* All config elements must be inside the <Config> element.
* All config elements shown above are mandatory.
* The numerical value started with `#` means that it is a decimal value.
* The numerical value started with `#x` means that it is a hexadecimal value.
* `<PeriodTime> ` element means that the period of control task is 10ms.
* `<MaxSafeStack>` means the stack size, and it is an estimated value. 8K is enough to satisfy most application.
* `<master_status_update_freq>` element means the frequency of masters status update. the value `#1` means  update the masters status every task period. 
* `<slave_status_update_freq>` element means the frequency of slaves status update. the value `#1` means  update the slaves status every task period. 
*` <axle_status_update_freq>` element means the frequency of axles status update. the value `#1`means  update the axles status every task period. 
* `<sync_ref_update_freq>` element means the frequency of reference clock update. the value `#2`means  update the axles status every two task period.
* `<is_xenomai>` element means whether Xenomai is supported.   the value `#1` means that Xenomai is supported on this host, and `#0` means not.
* `<sched_priority>` element means the priority of the user task.
* `<Masters>` element could contain more then one `Master` element .  For most cases, there is only one master on a host. 
* `<Axles>` element could contain more then one Axle element, which is the developer really care about.

#### Master Element

As `CoE network ` section shown, the Master could has many slaves, so the Master element will consist of  some `Slave` elements.


```
	<Master>
			<Master_index>#0</Master_index>
			<Reference_clock>#0</Reference_clock>
			<Slave  alias="#0" slave_position="#0">	
            ....
            </Slave>
            <Slave  alias="#1" slave_position="#1">	
            ....
            </Slave>
     </Master>   
```

* `<Master_index>` element means the index of the master. as mentioned above, for many cases, there is only one master, so the value of this element is always ` #0`. 

* `<Reference_clock>` element  is used to indicate which slave will be used the reference clock.

*  `<Slave>` element means there is a slave on this master.

#### Slave Element



	<Slave  alias="#0" slave_position="#0">				
				<VendorId>#x66668888</VendorId>
				<ProductCode>#x20181302</ProductCode>
				<Name>2HSS458-EC</Name>
				<Emerg_size>#x08</Emerg_size>
				<WatchDog>
					<Divider>#x0</Divider>
					<Intervals>#4000</Intervals>
				</WatchDog>	
				<DC> 
					<SYNC SubIndex='#0'> 
						<Shift>#0</Shift>
					</SYNC>
				</DC>		
				<SyncManagers force_pdo_assign="#1">
					<SyncManager SubIndex="#0">
						...
					</SyncManager>
			
					<SyncManager SubIndex="#1">
	                    ...
					</SyncManager>
				</SyncManagers>
				<Sdos>
					<Sdo>
						...
					</Sdo>
					<Sdo>
						...
					</Sdo>
				</Sdos>					
			</Slave>
* `alias` attribute means the alias name of this slave. 

* `slave_position`  attribute means which position of the slave is on this network. 

* `<Name>` element is the name of the slave.

* `<Emerg_size>` element is always 8 for all CoE device.

* `<WatchDog>`element is used to set the watch dog of this slave.

* `<DC>` element is used to set the sync info.

* `<SyncManagers>` element should contain all syncManager channels.  

* `<Sdos>` element contains the default value  we want to initiate by SDO channel.   

#### SyncManagers Element

For a CoE device, there are generally four syncManager channels. 

* SM0: Mailbox output

* SM1: Mailbox input

* SM2: Process data outputs

* SM3: process data inputs


```
	<SyncManager SubIndex="#0">
		<Index>#x1c10</Index>
		<Name>Sync Manager 0 </Name>
		<Dir>OUTPUT</Dir>
		<Watchdog>DISABLE</Watchdog>
		<PdoNum>#0</PdoNum>
	</SyncManager>
```
```
	<SyncManager SubIndex="#1">
		<Index>#x1c11</Index>
		<Name>Sync Manager 1</Name>
		<Dir>INPUT</Dir>
		<Watchdog>DISABLE</Watchdog>
		<PdoNum>#0</PdoNum>
	</SyncManager>
```
```
	<SyncManager SubIndex="#2">
		<Index>#x1c12</Index>
		<Name>Sync Manager 2</Name>
		<Dir>OUTPUT</Dir>
		<Watchdog>ENABLE</Watchdog>
		<PdoNum>#1</PdoNum>
		<Pdo SubIndex="#1">
			<Index>#x1600</Index>
			<Name>RxPdo 1</Name>							
			<Entry SubIndex="#1">
				...
			</Entry>
			<Entry SubIndex="#2">
				...
			</Entry>
		</Pdo>
	</SyncManager>
```
```
    <SyncManager SubIndex="#3">
		<Index>#x1c13</Index>
		<Name>Sync Manager 3</Name>
		<Dir>INPUT</Dir>
		<Watchdog>DISABLE</Watchdog>
		<PdoNum>#1</PdoNum>
		<Pdo SubIndex="#1">
			<Index>#x1a00</Index>
			<Name>TxPdo 1</Name>
			<Entry SubIndex="#1">
				...
			</Entry>
			<Entry SubIndex="#2">
				...
			</Entry>
										
		</Pdo>
	</SyncManager>
```

* `<Index>` element is the object address. 
* `<Name>` is a name of this syncmanager channel.  
* `<Dir>` element is the direction of  this syncmanager channel.
* `<Watchdog>` is used to set watchdog of  this syncmanager channel.
* `<PdoNum>` element means how many PDO we want to set.
* `<Pdo SubIndex="#1>` element contains the object dictionary entry we want to mapped.
  * `<Index>`PDO address.
  * `<Name>`PDO name
  * `<Entry>`  the object dictionary we want to mapped.

##### Entry Element

The Entry element is used to describe a object dictionary we want to mapped.

		<Entry SubIndex="#1">
			<Index>#x6041</Index>
			<SubIndex>#x0</SubIndex>
			<DataType>UINT</DataType>
			<BitLen>#16</BitLen>
			<Name>statusword</Name>
		</Entry>

#####  Sdo Element

The Sdo element is used to set the default value of a object dictionary.

```
	<Sdo>	
		<Index>#x6085</Index>
		<Subindex>#x0</Subindex>
		<value>#x1000</value>
		<BitLen>#32</BitLen>
		<DataType>DINT</DataType>
		<Name>Quick_stop_deceleration</Name>
	</Sdo>

```

The element shown in figure above means set the object dictionary "6085" to 0x1000.

#### Axle Element

```
	<Axle master_index='#0' slave_position="#0" AxleIndex="#0" AxleOffset="#0">
			<Mode>pp</Mode>
			<Name>x-axle</Name>
			<reg_pdo>
				...
			</reg_pdo>
			<reg_pdo>
				...
			</reg_pdo>
	</Axle>
```

* `master_index` attribute indicates which `master`  this `axle` belong to.

* `slave_position` attribute indicates which `slave` this `axle` belong to.

* `AxleOffset` attribute indicates which `axle` this `axle` is on the slave.  As mentioned above, a CoE slave could have more then on `axle`. If this axle is the second axle on the slave, set `AxleOffset="#1"`.

* `<Mode>` means which mode this axle will work on.

* `<Name>` is the name of this axle.

* `<reg_pdo>` is the PDO entry we want to register.

#####  reg_pdo element

```
	<reg_pdo>	
		<Index>#x606c</Index>
		<Subindex>#x0</Subindex>
		<Name></Name>
	</reg_pdo>			
```

## Test

##### Hardware preparation 

* A CoE servo system

  A CoE servo system includes a CoE servo and a motor. In this test, '2HSS458-EC' servo system shown as in figure below will be used.

* A board supported on OpenIL

  In this test, LS1046ARDB will be used.
  
  ![1576568047857](_images\1576568047857.png)

##### Software preparation

Make sure the below config options is selected when configuring OpenIL.

* BR2_PACKAGE_IGH_ETHERCAT=y
* BR2_PACKAGE_LIBXML2=y
* BR2_PACKAGE_NXP-SERVO=y

#####  CoE network Detection 

* Igh configuration
  * Configure the MASTER0_DEVICE field of the `/etc/ethercat.conf`
  
    Set MASTER0_DEVICE to the MAC address to indicate which port the Igh uses .
  
  * Configure DEVICE_MODULES="generic" of the `/etc/ethercat.conf`
  
* using the command `ethercatctl start` to start Igh service.

* Check CoE servo using below command.

  ```
  [root@OpenIL:~]#ethercat slaves
  0  0:0  PREOP  +  2HSS458-EC
  ```
##### Start test

_Note:_  The _Position encoder resolution_ and _Velocity encoder resolution_ of "2HSS458-EC" servo system are both 4000 . It means  the ratio of encoder increments per motor revolution. 

* _Profile Position_ mode test 

  * Start the test service as below.

    ````
    [root@OpenIL:~]# nservo_run -f /root/nservo_example/hss248_ec_config_pp.xml &
    ````
    
  * Check whether the status of the slave has been transferred from "PREOP" to "OP".

    ````
    [root@OpenIL:~]# ethercat slaves
    0  0:0  OP  +  2HSS458-EC
    ````

  * Check whether the phase of the master has been transferred from "Idle" to "Operation".

    ````
    [root@OpenIL:~]# ethercat master | grep Phase 
      Phase: Operation
    ````

  * Run below commands to test whether the motor works.

    * Get current mode of axle 0.
    
      ````
      [root@OpenIL:~]# nservo_client -a 0 -c get_mode 
      get_mode of the axle 0 : Profile Position Mode
      ````
      
    * Get current position of axle 0.
    
      ```
      [root@OpenIL:~]# nservo_client -a 0 -c get_position 
      get_current_position of the axle 0 : 0
      ```
      
    * Get the profile speed of axle 0.
    
      ```
      [root@OpenIL:~]# nservo_client -a 0  -c get_profile_speed
      get_profile_speed of the axle 0 : 800000
      ```
      
      The value 800000 means 200 revolutions per second. 
      
    * Set profile speed of axle 0.
    
      ```
      [root@OpenIL:~]# nservo_client -a 0  -c set_profile_speed:20000   
      set_profile_speed of the axle 0 : 20000
      ```
    
      Set profile speed to 5 revolutions per second.
    
    * Set target position of axle 0
    
      ````
      [root@OpenIL:~]# nservo_client -c set_position:400000
      set_position of the axle 0 : 400000
      ````
      
      The value 400000 means that the motor will turn 100 rounds.
      
       (target_position:400000 - current_position:0) / 4000 =  100 
      
    * Get current speed of axle 0
    
      ```
      [root@OpenIL:~]# nservo_client -a 0  -c get_speed
      get_speed of the axle 0 : 19999
      ```
    
    * Get target position of axle 0
    
      ```
      [root@OpenIL:~]# nservo_client -a 0  -c get_target_position
      get_target_position of the axle 0 : 400000
      ```
    
   * Exit test
  
     ```
     [root@OpenIL:~]# nservo_client  -c exit
     ```
  
* _Profile Velocity_ mode test 

  * Start the test service as below.

    ```
    [root@OpenIL:~]# nservo_run -f /root/nservo_example/hss248_ec_config_pv.xml &
    ```

  * Check whether the status of the slave has been transferred from "PREOP" to "OP".

    ````
    [root@OpenIL:~]# ethercat slaves
    0  0:0  OP  +  2HSS458-EC
    ````

  * Check whether the phase of the master has been transferred from "Idle" to "Operation".

    ````
    [root@OpenIL:~]# ethercat master | grep Phase 
      Phase: Operation
    ````

  * Run below commands to test whether the motor works.

    * Get current mode of axle 0.
    
      ````
      [root@OpenIL:~]# nservo_client -a 0  -c get_mode
      get_mode of the axle 0 : Profile Velocity Mode
      ````
      
    * Set target speed of axle 0
    
      ````
      [root@OpenIL:~]# nservo_client -a 0  -c set_speed:40000
      set_speed of the axle 0 : 40000
      ````
      
      The value 40000 means that the motor will turn with 10 revolutions per second.
      
    * Get current speed of axle 0
    
      ```
      [root@OpenIL:~]# nservo_client -a 0  -c get_speed
      get_speed of the axle 0 : 32000
      ```
    
    * Get target speed of axle 0
    
      ```
      [root@OpenIL:~]# nservo_client -a 0  -c get_target_speed
      get_target_speed of the axle 0 : 40000
      ```
   * Exit test
  
     ```
     [root@OpenIL:~]# nservo_client  -c exit
     ```
