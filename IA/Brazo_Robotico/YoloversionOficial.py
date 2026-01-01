import cv2
import time
import threading
import queue
import serial
import json
from ultralytics import YOLO

class SimpleESP32Cam:
    def __init__(self, url="http://192.168.4.1/video", buffer_size=2):
        self.url = url
        self.cap = None
        self.frame_queue = queue.Queue(maxsize=buffer_size)
        self.running = False
        self.capture_thread = None
        
    def start_capture(self):
        """Iniciar captura simple"""
        for attempt in range(3):
            try:
                print(f"🔗 Conectando cámara {attempt + 1}/3...")
                self.cap = cv2.VideoCapture(self.url)
                self.cap.set(cv2.CAP_PROP_BUFFERSIZE, 1)
                time.sleep(2)
                
                if self.cap.isOpened():
                    print("Cámara conectada")
                    self.running = True
                    self.capture_thread = threading.Thread(target=self._capture_worker)
                    self.capture_thread.daemon = True
                    self.capture_thread.start()
                    return True
                    
            except Exception as e:
                print(f"Error cámara: {e}")
            
            if attempt < 2:
                time.sleep(2)
        
        return False
        
    def _capture_worker(self):
        """Captura continua simple"""
        while self.running:
            try:
                ret, frame = self.cap.read()
                if ret:
                    if self.frame_queue.full():
                        try:
                            self.frame_queue.get_nowait()
                        except queue.Empty:
                            pass
                    self.frame_queue.put(frame)
                else:
                    time.sleep(0.01)
            except Exception:
                time.sleep(0.1)
                
    def get_frame(self):
        """Obtener frame"""
        try:
            return self.frame_queue.get_nowait()
        except queue.Empty:
            return None
            
    def stop(self):
        """Detener captura"""
        self.running = False
        if self.capture_thread:
            self.capture_thread.join(timeout=1.0)
        if self.cap:
            self.cap.release()

class SimpleArmController:
    def __init__(self, port='COM5', baudrate=115200):
        self.port = port
        self.baudrate = baudrate
        self.serial_conn = None
        self.connected = False
        
    def connect(self):
        try:
            print(f"🔧 Conectando brazo en {self.port}...")
            self.serial_conn = serial.Serial(self.port, self.baudrate, timeout=1)
            time.sleep(2)
            self.connected = True
            print("Brazo conectado")
            return True
        except Exception as e:
            print(f"Error conectando brazo: {e}")
            return False
            
    def send_command(self, command):
        if not self.connected:
            return False
            
        try:
            message = {"command": command}
            json_message = json.dumps(message) + '\n'
            print(f"ENVIANDO AL ARDUINO: {json_message.strip()}")
            self.serial_conn.write(json_message.encode('utf-8'))
            
            time.sleep(2.0)
            if self.serial_conn.in_waiting:
                response = self.serial_conn.read_all().decode().strip()
                if response:
                    print(f"RESPUESTA ARDUINO: {response}")
            return True
        except Exception as e:
            print(f"Error enviando comando: {e}")
            return False
            
    def pick_bad_fork(self):
        """Envía comando para mover tenedor defectuoso"""
        return self.send_command("pick_bad_fork")
        
    def reset_arm(self):
        return self.send_command("reset")
        
    def test_arm(self):
        return self.send_command("test_sequence")
        
    def calibrate_arm(self):
        return self.send_command("calibrate")
        
    def go_center(self):
        return self.send_command("go_center")
        
    def test_gripper(self):
        return self.send_command("test_gripper")
        
    def disconnect(self):
        if self.serial_conn:
            self.serial_conn.close()

def optimized_fork_classifier():
    """Clasificador optimizado - solo defectuosos a derecha"""
    print("CLASIFICADOR DE TENEDORES - SISTEMA COMPLETO")
    print("=" * 50)
    print("Configuración:")
    print("   - Solo detecta defectuosos en zona central")
    print("   - Defectuosos → Mover a derecha")
    print("   - Buenos → Ignorar")
    print("=" * 50)
    
    # Inicializar cámara
    cam = SimpleESP32Cam()
    
    if not cam.start_capture():
        print("No se pudo conectar cámara")
        return
    
    # Conectar brazo
    arm = SimpleArmController(port='COM5', baudrate=115200)
    if not arm.connect():
        print("Modo solo detección (sin brazo)")
    else:
        print("Sistema completo listo")
        print("Brazo configurado con movimiento SUAVE")
        print("   - Delay entre acciones: 15 segundos")
    
    # Cargar modelo YOLO
    print("Cargando modelo YOLO...")
    try:
        model = YOLO('runs/detect/train6/weights/best.pt')
        print("YOLO cargado correctamente")
    except Exception as e:
        print(f"Error cargando YOLO: {e}")
        cam.stop()
        arm.disconnect()
        return
    
    # Variables de control
    frame_count = 0
    start_time = time.time()
    last_action_time = 0
    action_delay = 15  # Delay entre acciones del brazo
    
    bad_moved = 0
    last_processed_frame = None
    
    print("\nIniciando sistema de clasificación...")
    print("Controles:")
    print("   - 'q' = Salir")
    print("   - 'r' = Resetear brazo") 
    print("   - 't' = Test brazo")
    print("   - 'c' = Calibrar brazo")
    print("   - 'g' = Ir al centro")
    print("   - 'p' = Test pinza")
    print("   - ESPACIO = Forzar detección manual")
    
    try:
        while True:
            current_time = time.time()
            
            # Obtener frame
            frame = cam.get_frame()
            
            if frame is not None:
                frame_count += 1
                current_frame = frame.copy()
                height, width = current_frame.shape[:2]
                
                # Procesar con YOLO cada 3 frames
                if frame_count % 3 == 0:
                    try:
                        results = model(current_frame, imgsz=320, conf=0.6, verbose=False)
                        detections = results[0]
                        
                        defective_in_center = False
                        confidence = 0.0
                        x_position = 0.0
                        
                        for box in detections.boxes:
                            cls = int(box.cls[0])
                            conf = float(box.conf[0])
                            
                            # Solo procesar tenedores defectuosos (clase 1)
                            if cls == 1 and conf > 0.6:  # Tenedor malo con buena confianza
                                x_center = float(box.xywh[0][0])
                                x_percent = (x_center / width) * 100
                                
                                # Solo si está en zona central (35%-65%)
                                if 35 <= x_percent <= 65:
                                    defective_in_center = True
                                    confidence = conf
                                    x_position = x_percent
                                    print(f"Defectuoso detectado! X: {x_percent:.1f}%, Conf: {conf:.2f}")
                                    break
                        
                        # Control del brazo - SOLO para defectuosos en centro
                        can_act = (current_time - last_action_time) > action_delay
                        if defective_in_center and arm.connected and can_act:
                            print(f"ENVIANDO COMANDO AL BRAZO...")
                            print(f"   Posición X: {x_position:.1f}%")
                            print(f"   Confianza: {confidence:.2f}")
                            
                            if arm.pick_bad_fork():
                                last_action_time = current_time
                                bad_moved += 1
                                print(f"Defectuosos movidos: {bad_moved}")
                                print(f"Esperando {action_delay}s para próxima acción...")
                            else:
                                print("Error enviando comando al brazo")
                        
                        # Dibujar detecciones
                        processed_frame = detections.plot()
                        last_processed_frame = processed_frame
                        
                    except Exception as e:
                        print(f"Error en procesamiento YOLO: {e}")
                        processed_frame = current_frame
                else:
                    # Usar frame cacheado
                    if last_processed_frame is not None:
                        processed_frame = last_processed_frame
                    else:
                        processed_frame = current_frame
                
                # Información en pantalla
                fps = frame_count / (current_time - start_time) if current_time > start_time else 0
                status_text = [
                    "SISTEMA CLASIFICADOR - DEFECTUOSOS",
                    f"FPS: {fps:.1f}",
                    f"Defectuosos movidos: {bad_moved}",
                    f"Brazo: {'CONECTADO' if arm.connected else 'DESCONECTADO'}",
                    f"Estado: {'LISTO' if (current_time - last_action_time) > action_delay else 'OCUPADO'}",
                    f"Tiempo espera: {max(0, action_delay - (current_time - last_action_time)):.1f}s"
                ]
                
                for i, text in enumerate(status_text):
                    if i == 0:  # Título
                        color = (255, 255, 0)  # Amarillo
                    elif "CONECTADO" in text and "LISTO" in text:
                        color = (0, 255, 0)  # Verde
                    elif "DESCONECTADO" in text or "OCUPADO" in text:
                        color = (0, 0, 255)  # Rojo
                    else:
                        color = (255, 255, 255)  # Blanco
                    
                    cv2.putText(processed_frame, text, (10, 30 + i * 25), 
                               cv2.FONT_HERSHEY_SIMPLEX, 0.6, color, 2)
                
                # Mostrar frame
                cv2.imshow('Clasificador de Tenedores - Defectuosos', processed_frame)
            
            # Control de teclas
            key = cv2.waitKey(1) & 0xFF
            if key == ord('q'):
                break
            elif key == ord('r') and arm.connected:
                print("Solicitando reset del brazo...")
                arm.reset_arm()
            elif key == ord('t') and arm.connected:
                print("Ejecutando test del brazo...")
                arm.test_arm()
            elif key == ord('c') and arm.connected:
                print("Calibrando brazo...")
                arm.calibrate_arm()
            elif key == ord('g') and arm.connected:
                print("Moviendo al centro...")
                arm.go_center()
            elif key == ord('p') and arm.connected:
                print("Probando pinza...")
                arm.test_gripper()
            elif key == ord(' ') and arm.connected:  # ESPACIO
                print("Forzando detección manual...")
                if arm.pick_bad_fork():
                    last_action_time = current_time
                    bad_moved += 1
                    print(f"Defectuosos movidos: {bad_moved}")
            
            time.sleep(0.01)
                
    except KeyboardInterrupt:
        print("\nSistema detenido por el usuario")
    except Exception as e:
        print(f"Error inesperado: {e}")
        import traceback
        traceback.print_exc()
    
    finally:
        # Limpieza
        print("\nCerrando sistema...")
        cam.stop()
        arm.disconnect()
        cv2.destroyAllWindows()
        
        # Estadísticas finales
        total_time = time.time() - start_time
        print(f"\n📊 ESTADÍSTICAS FINALES:")
        print(f"   Tiempo total: {total_time:.1f}s")
        print(f"   Frames procesados: {frame_count}")
        print(f"   FPS promedio: {frame_count/total_time:.1f}" if total_time > 0 else "   FPS: 0")
        print(f"   Tenedores defectuosos movidos: {bad_moved}")
        print("✅ Sistema finalizado correctamente")

if __name__ == "__main__":
    optimized_fork_classifier()