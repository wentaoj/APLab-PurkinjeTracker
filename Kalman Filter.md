## 1D Discrete Kalman Filter

estimate the state of a system based on observations or measurements.

### State Space Model
##### Linear State Equation (State Transition Matrix A, Control Matrix B)
$\textbf{x}_k = A\textbf{x}_{k-1} + B\textbf{u}_{k-1} + \textbf{w}_{k-1}$
- $\textbf{x}_k$: state vector at time $k$.
$$\textbf{x}_k = \begin{bmatrix} x_k \ \dot{x}_k \end{bmatrix}$$
- $\textbf{A}$: state transition matrix ($n \times n$).
$$A = \begin{bmatrix} 1 & \Delta{t} \\ 0 & 1 \end{bmatrix}$$
- $\textbf{B}$: control-input matrix ($n \times l$), applied to the control vector $\textbf{u}_{k-1}$.
$$B = \begin{bmatrix} \frac{1}{2}(\Delta{t})^2 \\ \Delta{t} \end{bmatrix}$$

##### Linear Observation Equation (Transformation Matrix H)
$\textbf{z}_k = H\textbf{x}_k + \textbf{v}_k$ 

##### parameters:
- $k$: time step (state of the system at time $k$), evolved from $k-1$.
- $\textbf{u}_k$: measurement model.
- $\textbf{A}$: state transition matrix ($n \times n$).
- $\textbf{B}$: control-input matrix ($n \times l$), applied to the control vector $\textbf{u}_{k-1}$.
- $\textbf{H}$: transformation matrix ($m \times n$), transforms the state into the measurement domain.
- $w_k$, $v_k$: process and measurement noise, with the covariance (confidence in the process model) $\textbf{Q}$ and the measurement noise vector with the *covariance* (confidence in the measurements) $\textbf{R}$; assumed statistically independence *Gaussian noise* with the *normal probability distribution*: 

$$p\left( w \right) \sim N(0, Q) \\
p\left( v \right)  \sim N(0, R)$$

#### Kalman Equations
Terms: 
- **priori**: estimates represent the predicted state $\hat{\mathbf{x}}^{-}_k$ and error covariance (confidence) $\mathbf{P}^{-}_k$ based on the system dynamics and the previously estimated state
- **posteriori**: estimates represent the state $\hat{\mathbf{x}}_k$ and error covariance $\mathbf{P}_k$ after incorporating the new measurement, which are referred to as the updated state and error covariance.

##### 0. Initialization
1. Initialize (Guess) the state estimate $\hat{\mathbf{x}}_0$ and the error covariance $\mathbf{P}_0$. 
    (Then, $\hat{\mathbf{x}}_{k-1}$ is initial state and error covariance matrix $\mathbf{P}_{k-1}$)

##### 1. Time Update Equations
Goal: Calculate the predicted state estimate (a priori state estimate) $\textbf { \^{x}}^{-}_k$ and predicted error covariance (a priori error covariance estimate) $\textbf {P}^{-}_k$.

1. Predicted (a priori) state estimate $\textbf { \^{x}}^{-}_k$: 
  $$ \hat{\mathbf{x}}^{-}_k = A\hat{\mathbf{x}}_{k-1} + Bu_{k-1} $$ 
  [Note: $ \textbf{\^{x}}_{k-1}$ is a posteriori state estimate].

2. Error covariance matrix $\textbf {P}^{-}_k$:
  $$ \textbf {P}^{-}_k = A \textbf{P}_{k-1}A^T+ \textbf{Q} $$ 
  [Note: $ \textbf{P}_{k-1}$ is the previous estimated error covariance matrix and $\textbf{Q}$ is the process noise covariance (Normal Distributed)].

##### 2. Measurement Update Equations
**Updated Stage**: 
1. Compute the Kalman Gain $\textbf{K}_k$ by 
$$ \textbf {K}_k = \textbf{P}^{-}_kH^T(H\textbf{P}^{-}_kH^T+ \textbf{R} )^{-1} $$
[Note: $H$ is the measurement matrix, $\textbf{R}$ is the measurement noise covariance matrix (Normal Distributed)].

2. Perform the actual measurement $ \textbf {z}_k $
   1. Update: $\textbf {\^{x}}^{-}_k$:
      - Measure the measurement residual: $$\textbf {z}_k - H\textbf{\^{x}} ^{-}_k$$ (or measurement innovation: Difference between the true measurement $\textbf {z}_k$ and the predicted measurement $H\textbf{\^{x}} ^{-}_k$)
   2. Update: Updated state estimed $\textbf {\^{x}}_k$: $$ \textbf {\^{x}}_k = \textbf{\^{x}}^{-}_k+ \textbf{K}_k(\textbf{z}_k-H \textbf{\^{x}} ^{-}_k) $$
   (performing the **summation** of the a priori projected state estimate $\textbf {\^{x}}^{-}_k$ to the product of the Kalman gain and the measurement residual.)
   3. Calculate: Updated error covariance matrix $\textbf{P}_k$ (a posteriori error covariance estimate):
      $$ \textbf{P}_k = (I - \textbf{K}_kH)\textbf{P}^{-}_k $$

---
![Cycle of Discrete Kalman Filter](https://machinelearningspace.com/wp-content/uploads/2020/02/Kalman.png)

------

## 2D Discrete Kalman Filter

### State Space Model

#### Recall: 1D Kinematic Equation
1. Based on Kinematic equation, the relation between the position $x$ and velocity $\dot x$ can be written as the following:

$$\textbf{x}_k = \begin{bmatrix} x_k\\ \dot x_k \\  \end{bmatrix} =  \begin{bmatrix} x_{k-1}+ \dot x_{k-1}\Delta t+ \frac{1}{2} \ddot x_{k-1}\Delta{t^2}\\  \dot x_{k-1}+ \ddot x_{k-1}\Delta{t} \\  \end{bmatrix}$$

$$\textbf{x}_k = \begin{bmatrix}x_k\\ \dot x_k \\  \end{bmatrix} =  \begin{bmatrix} 1 & \Delta{t}\\0 & 1\\ \end{bmatrix}   \begin{bmatrix}  x_{k-1} \\  \dot x_{k-1} \\ \end{bmatrix}  + \begin{bmatrix}  \frac{1}{2}(\Delta{t})^2 \\   \Delta{t} \\ \end{bmatrix}  \ddot x_{k-1}$$

#### 2-D Kinematic equation for state $\textbf{x}_k$
The state in time $k$ can be predicted by the previous state in time $k-1$. Let $x$ and $y$ be the positions in the $x–$ and $y-$directions, so $\dot x$ and $\dot y$ be the velocities and so $\ddot x$ and $\ddot y$ be accelerations in x– and y-directions.

$$\textbf{x}_k =  \begin{bmatrix}x_k\\y_k \\ \dot  x_k\\\dot  y_k \\  \end{bmatrix} = \begin{bmatrix}x_{k-1}+  \dot  x_{k-1}\Delta t+ \frac{1}{2}  \ddot  x_{k-1}\Delta{t^2}\\ y_{k-1}+  \dot y_{k-1}\Delta t+ \frac{1}{2}  \ddot  y_{k-1}\Delta{t^2}\\    \dot  x_{k-1}+  \ddot  x_{k-1}\Delta{t} \\   \dot  y_{k-1}+  \ddot  y_{k-1}\Delta{t} \\   \end{bmatrix}$$

$$\textbf{x}_k =  \begin{bmatrix}x_k\\y_k \\ \dot  x_k\\\dot  y_k \\      \end{bmatrix} =  \begin{bmatrix} 1 & 0 & \Delta{t} & 0 \\0 & 1  & 0 & \Delta{t}  \\ 0 & 0  & 1 & 0  \\  0 & 0  & 0 & 1  \\  \end{bmatrix}   \begin{bmatrix}  x_{k-1} \\  y_{k-1} \\ \dot  x_{k-1}  \\\dot  y_{k-1}  \\ \end{bmatrix}  + \begin{bmatrix}   \frac{1}{2}(\Delta{t})^2 & 0 \\ 0 & \frac{1}{2}(\Delta{t})^2 \\ \Delta{t}& 0 \\  0 & \Delta{t} \\  \end{bmatrix}   \begin{bmatrix}  \ddot  x_{k-1} \\ \ddot  y_{k-1} \\  \end{bmatrix}$$

$$\textbf{x}_k =  \begin{bmatrix} 1 & 0 & \Delta{t} & 0\\0 & 1  & 0 & \Delta{t}  \\ 0 & 0  & 1 & 0  \\  0 & 0  & 0 & 1  \\  \end{bmatrix}    \textbf{x}_{k-1} + \begin{bmatrix}  \frac{1}{2}(\Delta{t})^2 & 0 \\ 0 & \frac{1}{2}(\Delta{t})^2 \\ \Delta{t}& 0 \\  0 & \Delta{t} \\  \end{bmatrix} \textbf{a}_{k-1}$$

(Note: $\textbf{x}_k$ is the current state, $\textbf{x}_{k-1}$ is the previous state, and $\textbf{a}_ {k-1}$ is a vector of the previous acceleration in x– and y-directions.)

##### Determine the state transition matrix $\textbf{A}$ and the control-input matrix $\textbf{B}$:
$$\textbf{A} =  \begin{bmatrix} 1 & 0 & \Delta{t} & 0\\0 & 1  & 0 & \Delta{t}  \\ 0 & 0  & 1 & 0  \\  0 & 0  & 0 & 1  \\  \end{bmatrix}$$

$$\textbf{B} =  \begin{bmatrix}  \frac{1}{2}(\Delta{t})^2 & 0 \\ 0 & \frac{1}{2}(\Delta{t})^2 \\ \Delta{t}& 0 \\  0 & \Delta{t} \\  \end{bmatrix}$$

(Note: $\textbf{x}_k$ is the *state at time k* that composes of the positions $x_k$ and $y_k$ and the velocities $\dot  x_k$ and $\dot y_k$ at time $k$.)