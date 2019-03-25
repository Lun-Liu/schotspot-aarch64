class WWC1Array {
	int[] x = {0,0,0};
	int[] tmp = {0,0};

	class Thread1 extends Thread {
		public void run(){
			x[0] = 2;
		}
	}

	class Thread2 extends Thread {
		public void run(){
			tmp[0] = x[0];
			x[1] = 1;
		}
	}

	class Thread3 extends Thread {
		public void run(){
			tmp[1] = x[1];
			x[0] = 1;
		}
	}


	public void test(){
		Thread1 t1 = new Thread1();
		Thread2 t2 = new Thread2();
		Thread3 t3 = new Thread3();

		t1.start();
		t2.start();
		t3.start();

		try {
			t1.join();
			t2.join();
			t3.join();
		} catch(InterruptedException e) {
			System.out.println(e);
		}
	}


	public static void main(String[] args){
		int counter = 1;
		while (counter <= 100000){
			WWC1Array wwc = new WWC1Array();
			wwc.test();
			if(wwc.tmp[0] == 2 && wwc.tmp[1] == 1 && wwc.x[0] == 2) 
				break;
			counter++;
		}

		if(counter == 100001)
			System.out.println("ARM WWC Test PASSED after 100000 iterations");
		else
			System.out.println("ARM WWC Test FAILED: problem occurs on iteration " + counter);
	}
}
