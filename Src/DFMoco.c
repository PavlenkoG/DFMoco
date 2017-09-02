#include <stdio.h>
#include <string.h>
#include <stm32f0xx_hal.h>
#include <l6474.h>
#include <motorcontrol.h>
#include <system_config.h>
#include <stm32f0xx_nucleo_ihm01a1.h>

#include <DFMoco.h>

struct UserCmd processUserMessage(uint8_t *RxBuffer, uint8_t len);
void processSerialCommand(struct UserCmd);
void sendMessage(char *data_to_send, uint16_t len);
void processMessage(uint8_t MSG);
//void processGoPosition(int motorIndex, int32_t pos);
//void calculatePointToPoint(int motorIndex, int32_t destination);

struct txMsg {
    uint8_t msg;
    uint8_t motor;
};

struct TxMsgBuffer {
    struct txMsg buffer[TX_MSG_BUF_SIZE];
    uint8_t head;
    uint8_t tail;
};

struct TxMsgBuffer txMsgBuffer;

struct UserCmd {
    uint8_t command;
    uint8_t argCount;
    int32_t args[USER_CMD_ARGS];
} userCmd;

int msgState;
int32_t msgNumberSign;

extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef htimer2;
extern TIM_HandleTypeDef htimer16;

char ok_command[] = "hi 1 1 1.2.7 \r\n";
char err_comand[] = "command error \r\n";

struct UserCmd processUserMessage(uint8_t *RxBuffer, uint8_t len) {

    uint8_t data_present;
    uint8_t data;
    uint8_t lastUserData;

    data_present = 1;
    userCmd.command = 0;
    userCmd.argCount = 0;
    if (!(strncmp((char *) (RxBuffer), "hi", 2))) {	//
        userCmd.command = CMD_HI;
        data_present = 0;
    } else if (!(strncmp((char *) (RxBuffer), "ms", 2))) {	// motor status?
        userCmd.command = CMD_MS;
        data_present = 0;
    } else if (!(strncmp((char *) (RxBuffer), "sa", 2))) {	// stop all (hard)
        userCmd.command = CMD_SA;
        data_present = 0;
    } else if (!(strncmp((char *) (RxBuffer), "mm", 2))) {	// move motor
        userCmd.command = CMD_MM;
        data_present = 1;
    } else if (!(strncmp((char *) (RxBuffer), "np", 2))) {	//
        userCmd.command = CMD_NP;
        data_present = 1;
    } else if (!(strncmp((char *) (RxBuffer), "mp", 2))) {	// motor position
        userCmd.command = CMD_MP;
        data_present = 1;
    } else if (!(strncmp((char *) (RxBuffer), "zm", 2))) {	// zero motor
        userCmd.command = CMD_ZM;
        data_present = 1;
    } else if (!(strncmp((char *) (RxBuffer), "sm", 2))) {	// stop motor
        userCmd.command = CMD_SM;
        data_present = 1;
    } else if (!(strncmp((char *) (RxBuffer), "pr", 2))) {	// set pulse rate
        userCmd.command = CMD_PR;
        data_present = 1;
    } else if (!(strncmp((char *) (RxBuffer), "bf", 2))) {	// blur frame
        userCmd.command = CMD_BF;
        data_present = 1;
    } else if (!(strncmp((char *) (RxBuffer), "go", 2))) {	 // go!
        userCmd.command = CMD_GO;
        data_present = 1;
    } else if (!(strncmp((char *) (RxBuffer), "jm", 2))) {	 // jog motor
        userCmd.command = CMD_JM;
        data_present = 1;
    } else if (!(strncmp((char *) (RxBuffer), "im", 2))) {	 // inch motor
        userCmd.command = CMD_IM;
        data_present = 1;
    } else if (!(strncmp((char *) (RxBuffer), "in", 2))) {	// get status
        userCmd.command = CMD_GETSTAT;
        data_present = 0;
    } else if (!(strncmp((char *) (RxBuffer), "ss", 2))) {	//soft stop
        userCmd.command = CMD_SOFTSTOP;
        data_present = 0;
    } else if (!(strncmp((char *) (RxBuffer), "ga", 2))) {//get acceleration uint16_t
        userCmd.command = CMD_GET_ACCEL;
        data_present = 0;
    } else if (!(strncmp((char *) (RxBuffer), "gd", 2))) {//get decceleration uint16_t
        userCmd.command = CMD_GET_DECCEL;
        data_present = 0;
    } else if (!(strncmp((char *) (RxBuffer), "gs", 2))) {//get current speed uint16_t
        userCmd.command = CMD_GET_SPEED;
        data_present = 0;
    } else if (!(strncmp((char *) (RxBuffer), "gm", 2))) {	//get mark int32_t
        userCmd.command = CMD_GET_MARK;
        data_present = 0;
    } else if (!(strncmp((char *) (RxBuffer), "gx", 2))) {//get max speed uint16_t
        userCmd.command = CMD_GET_MAXSPEED;
        data_present = 0;
    } else if (!(strncmp((char *) (RxBuffer), "gi", 2))) {//get min speed uint16_t
        userCmd.command = CMD_GET_MINSPEED;
        data_present = 0;
    } else if (!(strncmp((char *) (RxBuffer), "gp", 2))) {//get current position int32_t
        userCmd.command = CMD_GET_POSITION;
        data_present = 0;
    } else if (!(strncmp((char *) (RxBuffer), "gh", 2))) {	//go home
        userCmd.command = CMD_GO_HOME;
        data_present = 0;
    } else if (!(strncmp((char *) (RxBuffer), "gg", 2))) {	//goto mark
        userCmd.command = CMD_GO_MARK;
        data_present = 0;
    } else if (!(strncmp((char *) (RxBuffer), "gt", 2))) {//goto position int32_t
        userCmd.command = CMD_GOTO;
    } else if (!(strncmp((char *) (RxBuffer), "hs", 2))) {	//hard stop
        userCmd.command = CMD_HARDSTOP;
        data_present = 0;
    } else if (!(strncmp((char *) (RxBuffer), "mo", 2))) {//move in direction for steps motorDir_t uint32_t
        userCmd.command = CMD_MOVE;
    }
    /*	else if (!(strncmp((char *)(RxBuffer),"qu",2))){//queue commands
     userCmd.command = CMD_QUEUECMD;
     }*/
    else if (!(strncmp((char *) (RxBuffer), "ru", 2))) {//run command direction motorDir_t
        userCmd.command = CMD_RUN;
    }
    /*	else if (!(strncmp((char *)(RxBuffer),"sq",2))){//send queue command
     userCmd.command = CMD_SENDQUEUE;
     }*/
    else if (!(strncmp((char *) (RxBuffer), "st", 2))) {//set acceleration uint16_t
        userCmd.command = CMD_SETACCEL;
    } else if (!(strncmp((char *) (RxBuffer), "sd", 2))) {//set decceleration uint16_t
        userCmd.command = CMD_SETDECCEL;
    } else if (!(strncmp((char *) (RxBuffer), "sn", 2))) {//set direction MotorDir_t
        userCmd.command = CMD_SETDIRECTION;
    } else if (!(strncmp((char *) (RxBuffer), "sh", 2))) {//set current position as home
        userCmd.command = CMD_SETHOME;
        data_present = 0;
    } else if (!(strncmp((char *) (RxBuffer), "sk", 2))) {//set current position as mark
        userCmd.command = CMD_SETMARK;
        data_present = 0;
    } else if (!(strncmp((char *) (RxBuffer), "sx", 2))) {//set max speed uint16_t
        userCmd.command = CMD_SETMAXSPEED;
    } else if (!(strncmp((char *) (RxBuffer), "si", 2))) {//set min speed uint16_t
        userCmd.command = CMD_SETMINSPEED;
    }

    RxBuffer = RxBuffer + 2;
    if (data_present) {
        if (len < 3)
            userCmd.command = CMD_NONE;
        len = len - 3;

        lastUserData = *RxBuffer;
        RxBuffer++;
        data = *RxBuffer;
        while (len) {
            if (((data >= '0' && data <= '9') || data == '-')
                    && lastUserData == ' ') {
                userCmd.argCount++;
                if (userCmd.argCount >= USER_CMD_ARGS) {
                    HAL_UART_Transmit(&huart2, (uint8_t*) (err_comand),
                            sizeof(err_comand) - 1, 100);
                    msgState = MSG_STATE_ERR;
                } else {
                    userCmd.args[userCmd.argCount - 1] = 0;
                    if (data == '-') {
                        msgNumberSign = -1;
                    } else {
                        msgNumberSign = 1;
                        userCmd.args[userCmd.argCount - 1] = (data - '0');
                    }
                }
            } else if (data >= '0' && data <= '9') {
                userCmd.args[userCmd.argCount - 1] =
                        userCmd.args[userCmd.argCount - 1] * 10 + (data - '0');
            }
            /*else*/if (/*data == ' ' ||*/len == 1) {
                if (lastUserData >= '0' && lastUserData <= '9') {
                    if (userCmd.argCount > 0)
                        userCmd.args[userCmd.argCount - 1] *= msgNumberSign;
                }
            }

            lastUserData = *RxBuffer;
            RxBuffer++;
            data = *RxBuffer;
            len--;
        }
    }

    return userCmd;
}

void processSerialCommand(struct UserCmd userCmd) {
    uint8_t motor;
    uint8_t parseError = 0;
    if (userCmd.command != CMD_NONE) {

        parseError = 0;
        motor = userCmd.args[0] - 1;

        switch (userCmd.command) {
        case CMD_HI:
            userCmd.args[0] = 1;
            processMessage(MSG_HI);
            processMessage(MSG_GETSTAT);
            processMessage(MSG_GET_POSITION);
            break;
        case CMD_PR:
            processMessage(MSG_PR);
            break;
        case CMD_MP:
            //HAL_TIM_OC_Start_IT(&htimer2, TIM_CHANNEL_1);
            processMessage(MSG_GET_POSITION);
            break;
        case CMD_MS:
            processMessage(MSG_GETSTAT);
            break;
        case CMD_MM:
            BSP_MotorControl_GoTo(0, userCmd.args[1]);
            processMessage(MSG_GOTO);
            break;
        case CMD_SM:
            BSP_MotorControl_HardStop(0);
            processMessage(MSG_HARDSTOP);
            processMessage(MSG_GET_POSITION);
            //processMessage (MSG_SM);
            break;
        case CMD_JM:
            if (userCmd.args[1] > 0)
                BSP_MotorControl_Run(0, 0);
            else
                BSP_MotorControl_Run(0, 1);
            processMessage(MSG_JM);
        case CMD_GETSTAT:
            processMessage(MSG_GETSTAT);
            break;
        case CMD_GOTODIR:
            //not present in l6747 driver
            //BSP_MotorControl_CmdGoToDir(0,userCmd.args[0],userCmd.args[1]);
            processMessage(MSG_GOTODIR);
            break;
        case CMD_SOFTSTOP:
            BSP_MotorControl_SoftStop(0);
            processMessage(MSG_SOFTSTOP);
            break;
        case CMD_GET_ACCEL:
            processMessage(MSG_GET_ACCEL);
            break;
        case CMD_GET_DECCEL:
            processMessage(MSG_GET_DECCEL);
            break;
        case CMD_GET_SPEED:
            processMessage(MSG_GET_SPEED);
            break;
        case CMD_GET_MARK:
            processMessage(MSG_GET_MARK);
            break;
        case CMD_GET_MAXSPEED:
            processMessage(MSG_GET_MAXSPEED);
            break;
        case CMD_GET_MINSPEED:
            processMessage(MSG_GET_MINSPEED);
            break;
        case CMD_GET_POSITION:
            processMessage(MSG_GET_POSITION);
            break;
        case CMD_GO_HOME:
            BSP_MotorControl_GoHome(0);
            processMessage(MSG_GO_HOME);
            break;
        case CMD_GO_MARK:
            BSP_MotorControl_GoMark(0);
            processMessage(MSG_GO_MARK);
            break;
        case CMD_GOTO:
            BSP_MotorControl_GoTo(0, userCmd.args[0]);
            processMessage(MSG_GOTO);
            break;
        case CMD_HARDSTOP:
            BSP_MotorControl_HardStop(0);
            processMessage(MSG_HARDSTOP);
            break;
        case CMD_MOVE:
            BSP_MotorControl_Move(0, userCmd.args[0], userCmd.args[1]);
            processMessage(MSG_MOVE);
            break;
            /*				case CMD_QUEUECMD:
             break;*/
        case CMD_RUN:
            BSP_MotorControl_Run(0, userCmd.args[0]);
            processMessage(MSG_RUN);
            break;
            /*				case CMD_SENDQUEUE:
             break;*/
        case CMD_SETACCEL:
            BSP_MotorControl_SetAcceleration(0, (uint16_t) userCmd.args[0]);
            processMessage(MSG_GET_ACCEL);
            break;
        case CMD_SETDECCEL:
            BSP_MotorControl_SetDeceleration(0, (uint16_t) userCmd.args[0]);
            processMessage(MSG_GET_DECCEL);
            break;
            break;
        case CMD_SETDIRECTION:
            break;
        case CMD_SETHOME:
            BSP_MotorControl_SetHome(0);
            processMessage(MSG_SETHOME);
            break;
        case CMD_SETMARK:
            BSP_MotorControl_SetMark(0);
            processMessage(MSG_SETMARK);
            break;
        case CMD_SETMAXSPEED:
            BSP_MotorControl_SetMaxSpeed(0, (uint16_t) userCmd.args[0]);
            processMessage(MSG_GET_MAXSPEED);
            break;
        case CMD_SETMINSPEED:
            BSP_MotorControl_SetMinSpeed(0, (uint16_t) userCmd.args[0]);
            processMessage(MSG_GET_MINSPEED);
            break;
        }
    }
}

void sendMessage(char *data_to_send, uint16_t len) {

    HAL_UART_Transmit(&huart2, (uint8_t*) (data_to_send), len, 100);
}
void processMessage(uint8_t MSG) {
    int32_t param_32t;
    int32_t param_32t_c_len;
    uint16_t param_16t;
    uint16_t param_16t_c_len;
    bool int32_active;
    char parameter_c[50];
    switch (MSG) {
    case MSG_HI:
        int32_active = 0;
        param_16t_c_len = sprintf(parameter_c, "hi 1 4 1.2.7 \r\n");
        break;
    case MSG_PR:
        int32_active = 0;
        param_16t_c_len = sprintf(parameter_c, "pr 1 %d\r\n", userCmd.args[1]);
        break;
    case MSG_JM:
        int32_active = 0;
        param_16t_c_len = sprintf(parameter_c, "jm 1\r\n");
        break;
    case MSG_GETSTAT:
        int32_active = 0;
        param_16t = BSP_MotorControl_GetDeviceState(0);
        /*if (param_16t == ACCELERATING) param_16t_c_len = sprintf(parameter_c, "Motor is Accelerating\r\n");
         else if (param_16t == DECELERATING) param_16t_c_len = sprintf(parameter_c, "Motor is Deccelerating\r\n");
         else if (param_16t == STEADY) param_16t_c_len = sprintf(parameter_c, "Motor is Steady\r\n");*/

        //else if (param_16t == INACTIVE) param_16t_c_len = sprintf(parameter_c, "Motor is Inactive\r\n");
        if (param_16t == INACTIVE)
            param_16t_c_len = sprintf(parameter_c, "ms 0000\r\n");
        else
            param_16t_c_len = sprintf(parameter_c, "ms 1000\r\n");
        //else param_16t_c_len = sprintf(parameter_c, "Motor not found\r\n");
        break;
    case MSG_GOTODIR:
        int32_active = 1;
        param_32t_c_len = sprintf(parameter_c, "Motor is moving to %d\r\n",
                userCmd.args[3]);
        break;
    case MSG_SOFTSTOP:
        int32_active = 0;
        param_16t_c_len = sprintf(parameter_c, "Motor soft stop\r\n");
        break;
    case MSG_GET_ACCEL:
        int32_active = 0;
        param_16t = BSP_MotorControl_GetAcceleration(0);
        param_16t_c_len = sprintf(parameter_c, "Motor Acceleration = %d\r\n",
                param_16t);
        break;
    case MSG_GET_DECCEL:
        int32_active = 0;
        param_16t = BSP_MotorControl_GetDeceleration(0);
        param_16t_c_len = sprintf(parameter_c, "Motor Deceleration = %d\r\n",
                param_16t);
        break;
    case MSG_GET_SPEED:
        int32_active = 0;
        param_16t = BSP_MotorControl_GetCurrentSpeed(0);
        param_16t_c_len = sprintf(parameter_c, "Motor Current Speed = %d\r\n",
                param_16t);
        break;
    case MSG_GET_MARK:
        int32_active = 1;
        param_32t = BSP_MotorControl_GetMark(0);
        param_32t_c_len = sprintf(parameter_c, "Mark = %d\r\n",
                userCmd.args[3]);
        break;
    case MSG_GET_MAXSPEED:
        int32_active = 0;
        param_16t = BSP_MotorControl_GetMaxSpeed(0);
        param_16t_c_len = sprintf(parameter_c, "Motor max speed = %d\r\n",
                param_16t);
        break;
    case MSG_GET_MINSPEED:
        int32_active = 0;
        param_16t = BSP_MotorControl_GetMinSpeed(0);
        param_16t_c_len = sprintf(parameter_c, "Motor min speed = %d\r\n",
                param_16t);
        break;
    case MSG_GET_POSITION:
        int32_active = 1;
        param_32t = BSP_MotorControl_GetPosition(0);
        if (userCmd.args[0] == 0)
            userCmd.args[0] = 1;
        param_32t_c_len = sprintf(parameter_c, "mp %d %d\r\n", userCmd.args[0],
                param_32t);
        //param_32t_c_len = sprintf(parameter_c, "Current Position = %d\r\n",param_32t);
        break;
    case MSG_GO_HOME:
        int32_active = 0;
        param_16t_c_len = sprintf(parameter_c, "Motor is going home\r\n");
        break;
    case MSG_GO_MARK:
        int32_active = 0;
        param_16t_c_len = sprintf(parameter_c,
                "Motor is going to home position\r\n");
        break;
    case MSG_GOTO:
        int32_active = 1;
        //param_32t_c_len = sprintf(parameter_c, "Motor is going to position %d\r\n",userCmd.args[0]);
        param_32t_c_len = sprintf(parameter_c, "mm 1 %d\r\n", userCmd.args[1]);
        break;
    case MSG_HARDSTOP:
        int32_active = 0;
        param_16t_c_len = sprintf(parameter_c, "sm 1\r\n");
        //param_16t_c_len = sprintf(parameter_c, "Motor is stooped\r\n");
        break;
    case MSG_MOVE:
        int32_active = 1;
        param_16t_c_len = sprintf(parameter_c,
                "Motor is moving in directoin %d for %d steps\r\n",
                userCmd.args[0], userCmd.args[1]);
        break;
    case MSG_RUN:
        int32_active = 0;
        param_16t_c_len = sprintf(parameter_c,
                "Motor is running to direction %d\r\n",
                (uint16_t) userCmd.args[0]);
        break;
        /*		case MSG_SETACCEL	:
         int32_active = 0;
         param_16t_c_len = sprintf(parameter_c, "Acceleration set to %d\r\n",(uint16_t)userCmd.args[0]);
         break;
         case MSG_SETDECCEL:
         int32_active = 0;
         param_16t_c_len = sprintf(parameter_c, "Acceleration set to %d\r\n",(uint16_t)userCmd.args[0]);
         break;*/
    case MSG_SETHOME:
        int32_active = 0;
        param_16t_c_len = sprintf(parameter_c,
                "Current position is set as HOME\r\n");
        break;
    case MSG_SETMARK:
        int32_active = 0;
        param_16t_c_len = sprintf(parameter_c,
                "Current position is marked\r\n");
        break;

    }
    if (int32_active)
        HAL_UART_Transmit(&huart2, (uint8_t*) (parameter_c), param_32t_c_len,
                100);
    else
        HAL_UART_Transmit(&huart2, (uint8_t*) (parameter_c), param_16t_c_len,
                100);
    userCmd.args[0] = 0;
    userCmd.args[1] = 0;

}

