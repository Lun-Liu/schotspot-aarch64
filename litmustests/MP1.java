class MP1 {
	int x = 0;
	int y = 0;
	int tmpx = 0;
	int tmpy = 0;

	class Thread1 extends Thread {
		public void run(){
			x = 1;
			y = 1;
		}
	}

	class Thread2 extends Thread {
		public void run(){
			tmpy = y;
			tmpx = x;
		}
	}

	public void test(){
		Thread1 t1 = new Thread1();
		Thread2 t2 = new Thread2();
		t1.start();
		t2.start();

		try {
			t1.join();
			t2.join();
		} catch(InterruptedException e) {
			System.out.println(e);
		}
	}


	public static void main(String[] args){
		int counter = 1;
		while (counter <= 100000){
			MP1 mp = new MP1();
			mp.test();
			if(mp.tmpy == 1 && mp.tmpx == 0)
				break;
			counter++;
		}

		if(counter == 100001)
			System.out.println("ARM MP Test PASSED after 100000 iterations");
		else
			System.out.println("ARM MP Test FAILED: problem occurs on iteration " + counter);
	}
}
